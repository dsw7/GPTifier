#include "command_run.hpp"

#include "chat_completions.hpp"
#include "configs.hpp"
#include "datadir.hpp"
#include "utils.hpp"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

void help_run()
{
    const std::string messages = R"(Create a chat completion according to a prompt. A prompt can be provided
as follows:

  1. Via a file named 'Inputfile' in the current working directory
  2. Via a file with a custom name (see -r option)
  3. Via command line (see -p option)
  4. Interactively via stdin

Usage:
  gpt run [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid chat model
  -o, --file=FILE                Export results to a JSON file named FILE
  -p, --prompt=PROMPT            Provide prompt via command line
  -r, --read-from-file=FILENAME  Read prompt from a custom file named FILENAME
  -t, --temperature=TEMPERATURE  Provide a sampling temperature between 0 and 2
  -s, --store-completion         Store results of completion on OpenAI servers

Examples:
  > Run an interaction session:
    $ gpt run
  > Run a query non-interactively and export results
    $ gpt run --prompt="What is 3 + 5?" --file="/tmp/results.json"
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    bool store_completion = false;
    std::optional<std::string> json_dump_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> prompt = std::nullopt;
    std::optional<std::string> prompt_file = std::nullopt;
    std::string temperature = "1.00";
};

Parameters read_cli(int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
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
                help_run();
                exit(EXIT_SUCCESS);
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
                utils::exit_on_failure();
        }
    };

    if (params.json_dump_file) {
        if (params.json_dump_file.value().empty()) {
            throw std::runtime_error("No filename provided");
        }
    }

    if (params.prompt_file) {
        if (params.prompt_file.value().empty()) {
            throw std::runtime_error("Empty prompt filename");
        }
    }

    if (params.temperature.empty()) {
        throw std::runtime_error("Empty temperature");
    }

    return params;
}

std::string get_model()
{
#ifdef TESTING_ENABLED
    static std::string low_cost_model = "gpt-3.5-turbo";
    return low_cost_model;
#else
    if (configs.model_run) {
        return configs.model_run.value();
    }
    throw std::runtime_error("Could not determine which model to use");
#endif
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

std::atomic<bool> TIMER_ENABLED(false);

void time_api_call()
{
    auto delay = std::chrono::milliseconds(50);
    int counter = 0;

    while (TIMER_ENABLED.load()) {
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

serialization::ChatCompletion run_query(const std::string &model, const std::string &prompt, const float temperature, const bool store_completion)
{
    TIMER_ENABLED.store(true);
    std::thread timer(time_api_call);

    bool query_failed = false;
    serialization::ChatCompletion cc;
    std::mutex mutex_print_stderr;

    try {
        cc = serialization::create_chat_completion(prompt, model, temperature, store_completion);
    } catch (std::runtime_error &e) {
        query_failed = true;
        {
            std::lock_guard<std::mutex> lock(mutex_print_stderr);
            fmt::print(stderr, "{}\n", e.what());
        }
    }

    TIMER_ENABLED.store(false);
    timer.join();

    if (query_failed) {
        throw std::runtime_error("Cannot proceed");
    }

    return cc;
}

// Output ---------------------------------------------------------------------------------------------------

void dump_chat_completion_response(const serialization::ChatCompletion &cc, const std::string &json_dump_file)
{

    const nlohmann::json json = {
        { "completion", cc.completion },
        { "completion_tokens", cc.completion_tokens },
        { "created", cc.created },
        { "id", cc.id },
        { "model", cc.model },
        { "prompt", cc.prompt },
        { "prompt_tokens", cc.prompt_tokens },
        { "rtt", cc.rtt.count() },
    };

    fmt::print("Dumping results to '{}'\n", json_dump_file);
    utils::write_to_file(json_dump_file, json.dump(2));
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

void print_usage_statistics(const serialization::ChatCompletion &completion)
{
    const int wc_prompt = utils::get_word_count(completion.prompt);
    const int wc_completion = utils::get_word_count(completion.completion);

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

void write_message_to_file(const serialization::ChatCompletion &completion)
{
    const std::string created = utils::datetime_from_unix_timestamp(completion.created);

    std::string text = "{\n";
    text += "> Created at: " + created + " (GMT)\n";
    text += "> Model: " + completion.model + "\n\n";
    text += "> Prompt:\n" + completion.prompt + "\n\n";
    text += "> Completion:\n" + completion.completion + "\n";
    text += "}\n\n";

    const std::string path_completions_file = datadir::GPT_COMPLETIONS.string();
    fmt::print("> Writing completion to file {}\n", path_completions_file);

    utils::append_to_file(path_completions_file, text);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

void export_chat_completion_response(const serialization::ChatCompletion &completion)
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

#pragma GCC diagnostic pop

} // namespace

namespace commands {

void command_run(int argc, char **argv)
{
    const Parameters params = read_cli(argc, argv);
    utils::separator();

    std::string model;
    if (params.model) {
        model = params.model.value();
    } else {
        model = get_model();
    }

    if (model.empty()) {
        throw std::runtime_error("Model is empty");
    }

    const std::filesystem::path inputfile = std::filesystem::current_path() / "Inputfile";
    std::string prompt;

    if (params.prompt) {
        prompt = params.prompt.value();
    } else {
        if (params.prompt_file) {
            fmt::print("Reading text from file: '{}'\n", params.prompt_file.value());
            prompt = utils::read_from_file(params.prompt_file.value());
        } else if (std::filesystem::exists(inputfile)) {
            fmt::print("Found an Inputfile in current working directory!\n");
            prompt = utils::read_from_file(inputfile);
        } else {
            prompt = read_text_from_stdin();
        }
        utils::separator();
    }

    if (prompt.empty()) {
        throw std::runtime_error("No input text provided anywhere. Cannot proceed");
    }

    const float temperature = utils::string_to_float(params.temperature);
    if (temperature < 0 || temperature > 2) {
        throw std::runtime_error("Temperature must be between 0 and 2");
    }

    const serialization::ChatCompletion cc = run_query(model, prompt, temperature, params.store_completion);

    if (params.json_dump_file) {
        dump_chat_completion_response(cc, params.json_dump_file.value());
        return;
    }

    print_usage_statistics(cc);
    utils::separator();

    print_chat_completion_response(cc.completion);
    utils::separator();

#ifndef TESTING_ENABLED
    export_chat_completion_response(cc);
    utils::separator();
#endif
}

} // namespace commands
