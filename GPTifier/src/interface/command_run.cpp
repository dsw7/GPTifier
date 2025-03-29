#include "interface/command_run.hpp"

#include "datadir.hpp"
#include "help_messages.hpp"
#include "params.hpp"
#include "selectors.hpp"
#include "serialization/chat_completions.hpp"
#include "utils.hpp"

#include <chrono>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

ParamsRun read_cli(int argc, char **argv)
{
    ParamsRun params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "no-interactive-export", no_argument, 0, 'u' },
            { "store-completion", no_argument, 0, 's' },
            { "file", required_argument, 0, 'o' },
            { "model", required_argument, 0, 'm' },
            { "prompt", required_argument, 0, 'p' },
            { "read-from-file", required_argument, 0, 'r' },
            { "temperature", required_argument, 0, 't' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "huso:m:p:r:t:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                cli::help_command_run();
                exit(EXIT_SUCCESS);
            case 'u':
                params.enable_export = false;
                break;
            case 's':
                params.store_completion = true;
                break;
            case 'o':
                params.json_dump_file = optarg;
                break;
            case 'p':
                params.prompt = optarg;
                break;
            case 't':
                params.temperature = optarg;
                break;
            case 'r':
                params.prompt_file = optarg;
                break;
            case 'm':
                params.model = optarg;
                break;
            default:
                cli::exit_on_failure();
        }
    };

    params.sanitize();
    return params;
}

// Input ----------------------------------------------------------------------------------------------------

std::string read_text_from_stdin()
{
    fmt::print(fg(white), "Input: ");
    std::string text;

    std::getline(std::cin, text);
    return text;
}

// Completion -----------------------------------------------------------------------------------------------

bool TIMER_ENABLED = false;

void time_api_call()
{
    auto delay = std::chrono::milliseconds(50);
    int counter = 0;

    while (TIMER_ENABLED) {
        switch (counter) {
            case 0:
                std::cout << "·....\r" << std::flush;
                break;
            case 5:
                std::cout << ".·...\r" << std::flush;
                break;
            case 10:
                std::cout << "..·..\r" << std::flush;
                break;
            case 15:
                std::cout << "...·.\r" << std::flush;
                break;
            case 20:
                std::cout << "....·\r" << std::flush;
                break;
        }
        counter++;

        if (counter > 24) {
            counter = 0;
        }

        std::this_thread::sleep_for(delay);
    }

    std::cout << std::string(16, ' ') << '\r' << std::flush;
}

ChatCompletion run_query(const std::string &model, const std::string &prompt, float temperature, bool store_completion)
{
    TIMER_ENABLED = true;
    std::thread timer(time_api_call);

    bool query_failed = false;
    ChatCompletion cc;

    try {
        cc = create_chat_completion(prompt, model, temperature, store_completion);
    } catch (std::runtime_error &e) {
        query_failed = true;
        fmt::print(stderr, "{}\n", e.what());
    }

    TIMER_ENABLED = false;
    timer.join();

    if (query_failed) {
        throw std::runtime_error("Cannot proceed");
    }

    return cc;
}

// Output ---------------------------------------------------------------------------------------------------

void dump_chat_completion_response(const ChatCompletion &cc, const std::string &json_dump_file)
{
    fmt::print("Dumping results to '{}'\n", json_dump_file);
    std::ofstream st_filename(json_dump_file);

    if (not st_filename.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'\n", json_dump_file);
        throw std::runtime_error(errmsg);
    }

    nlohmann::json json;
    json["completion"] = cc.completion;
    json["completion_tokens"] = cc.completion_tokens;
    json["created"] = cc.created;
    json["id"] = cc.id;
    json["model"] = cc.model;
    json["prompt"] = cc.prompt;
    json["prompt_tokens"] = cc.prompt_tokens;
    json["rtt"] = cc.rtt.count();

    st_filename << json.dump(2);
    st_filename.close();
}

void print_chat_completion_response(const std::string &completion)
{
    fmt::print(fg(white), "Results: ");
    fmt::print(fg(green), "{}\n", completion);
}

void print_ratio(int num_tokens, int num_words)
{
    if (num_words < 1) {
        throw std::runtime_error("Zero division. Number of words is 0!");
    }

    // The ideal ratio is about 100 tokens / 75 words
    // See https://platform.openai.com/tokenizer for more information
    float ratio = (float)num_tokens / num_words;

    fmt::print("Ratio: ");

    if (ratio <= 2) {
        fmt::print(fg(green), "{}\n", ratio);
    } else if (ratio > 2 and ratio <= 3) {
        fmt::print(fg(yellow), "{}\n", ratio);
    } else {
        fmt::print(fg(red), "{}\n", ratio);
    }
}

void print_usage_statistics(const ChatCompletion &completion)
{
    int wc_prompt = get_word_count(completion.prompt);
    int wc_completion = get_word_count(completion.completion);

    fmt::print(fg(white), "Usage:\n");
    fmt::print("Model: {}\n", completion.model);
    fmt::print("RTT: {} s\n", completion.rtt.count());
    fmt::print("\n");

    fmt::print("Prompt tokens: ");
    fmt::print(fg(green), "{}\n", completion.prompt_tokens);
    fmt::print("Prompt size (words): ");
    fmt::print(fg(green), "{}\n", wc_prompt);
    print_ratio(completion.prompt_tokens, wc_prompt);
    fmt::print("\n");

    fmt::print("Completion tokens: ");
    fmt::print(fg(green), "{}\n", completion.completion_tokens);
    fmt::print("Completion size (words): ");
    fmt::print(fg(green), "{}\n", wc_completion);
    print_ratio(completion.completion_tokens, wc_completion);
}

void write_message_to_file(const ChatCompletion &completion)
{
    const std::string path_completions_file = datadir::GPT_COMPLETIONS.string();

    fmt::print("> Writing completion to file {}\n", path_completions_file);
    std::ofstream st_filename(path_completions_file, std::ios::app);

    if (not st_filename.is_open()) {
        throw std::runtime_error("Unable to open " + path_completions_file);
    }

    const std::string created = datetime_from_unix_timestamp(completion.created);

    st_filename << "{\n";
    st_filename << "> Created at: " + created + " (GMT)\n";
    st_filename << "> Model: " + completion.model + "\n\n";
    st_filename << "> Prompt:\n"
                << completion.prompt << "\n\n";
    st_filename << "> Completion:\n"
                << completion.completion << "\n";
    st_filename << "}\n\n";

    st_filename.close();
}

void export_chat_completion_response(const ChatCompletion &completion)
{
    fmt::print(fg(white), "Export:\n");
    char choice = 'n';

    while (true) {
        std::cout << "> Write reply to file? [y/n]: ";
        choice = std::cin.get();

        if (choice == 'y' or choice == 'n') {
            break;
        } else {
            std::cout << "> Invalid choice. Input either 'y' or 'n'!\n";
        }
    }

    if (choice == 'n') {
        std::cout << "> Not exporting response.\n";
        return;
    }

    write_message_to_file(completion);
}

} // namespace

void command_run(int argc, char **argv)
{
    ParamsRun params = read_cli(argc, argv);
    print_sep();

    static std::filesystem::path inputfile = std::filesystem::current_path() / "Inputfile";
    std::string prompt;

    if (params.prompt.has_value()) {
        prompt = params.prompt.value();
    } else {
        if (params.prompt_file.has_value()) {
            fmt::print("Reading text from file: '{}'\n", params.prompt_file.value());
            prompt = read_text_from_file(params.prompt_file.value());
        } else if (std::filesystem::exists(inputfile)) {
            fmt::print("Found an Inputfile in current working directory!\n");
            prompt = read_text_from_file(inputfile);
        } else {
            prompt = read_text_from_stdin();
        }
        print_sep();
    }

    if (prompt.empty()) {
        throw std::runtime_error("No input text provided anywhere. Cannot proceed");
    }

    std::string model;

    if (params.model.has_value()) {
        model = params.model.value();
    } else {
        model = select_chat_model();
    }

    float temperature = 1.00;
    if (std::holds_alternative<float>(params.temperature)) {
        temperature = std::get<float>(params.temperature);
    }

    const ChatCompletion cc = run_query(model, prompt, temperature, params.store_completion);

    if (params.json_dump_file.has_value()) {
        dump_chat_completion_response(cc, params.json_dump_file.value());
        return;
    }

    print_usage_statistics(cc);
    print_sep();

    print_chat_completion_response(cc.completion);
    print_sep();

    if (params.enable_export) {
        export_chat_completion_response(cc);
        print_sep();
    }
}
