#include "command_run.hpp"

#include "api_openai_user.hpp"
#include "cli.hpp"
#include "datadir.hpp"
#include "models.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "selectors.hpp"
#include "utils.hpp"
#include "validation.hpp"

#include <chrono>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <thread>

using json = nlohmann::json;

namespace {

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
    auto delay = std::chrono::milliseconds(250);
    auto start = std::chrono::high_resolution_clock::now();

    while (TIMER_ENABLED) {
        std::this_thread::sleep_for(delay);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        fmt::print(fg(white), "Time (s): ");
        fmt::print("{}\r", duration.count());
        std::cout.flush();
    }

    std::cout << "\n";
}

void create_chat_completion(models::Completion &completion, const std::string &model, const std::string &prompt, float temperature)
{
    const json messages = { { "role", "user" }, { "content", prompt } };
    const json data = {
        { "model", model }, { "temperature", temperature }, { "messages", json::array({ messages }) }
    };

    OpenAIUser api;
    const std::string response = api.create_chat_completion(data.dump());
    const json results = parse_response(response);

    if (not validation::is_chat_completion(results)) {
        throw std::runtime_error("Response from OpenAI is not a chat completion");
    }

    completion.completion_tokens = results["usage"]["completion_tokens"];
    completion.completion = results["choices"][0]["message"]["content"];
    completion.created = results["created"];
    completion.model = results["model"];
    completion.prompt = prompt;
    completion.prompt_tokens = results["usage"]["prompt_tokens"];
}

models::Completion run_query(const std::string &model, const std::string &prompt, float temperature)
{
    TIMER_ENABLED = true;
    std::thread timer(time_api_call);

    bool query_failed = false;
    models::Completion completion;

    try {
        create_chat_completion(completion, model, prompt, temperature);
    } catch (std::runtime_error &e) {
        query_failed = true;
        fmt::print(stderr, "{}\n", e.what());
    }

    TIMER_ENABLED = false;
    timer.join();

    if (query_failed) {
        throw std::runtime_error("Cannot proceed");
    }

    return completion;
}

// Output ---------------------------------------------------------------------------------------------------

void dump_chat_completion_response(const models::Completion &completion, const std::string &json_dump_file)
{
    fmt::print("Dumping results to '{}'\n", json_dump_file);
    std::ofstream st_filename(json_dump_file);

    if (not st_filename.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'\n", json_dump_file);
        throw std::runtime_error(errmsg);
    }

    st_filename << std::setw(2) << completion.jsonify();
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

void print_usage_statistics(const models::Completion &completion)
{
    int wc_prompt = get_word_count(completion.prompt);
    int wc_completion = get_word_count(completion.completion);

    fmt::print(fg(white), "Usage:\n");
    fmt::print("Model: {}\n", completion.model);
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

void write_message_to_file(const models::Completion &completion)
{
    const std::string path_completions_file = datadir::GPT_COMPLETIONS.string();

    fmt::print("> Writing completion to file {}\n", path_completions_file);
    std::ofstream st_filename(path_completions_file, std::ios::app);

    if (not st_filename.is_open()) {
        throw std::runtime_error("Unable to open " + path_completions_file);
    }

    const std::string created = datetime_from_unix_timestamp(completion.created);

    static int column_width = 110;
    std::string sep_outer(column_width, '=');
    std::string sep_inner(column_width, '-');

    st_filename << sep_outer + '\n';
    st_filename << "Created at: " + created + " (GMT) \n";
    st_filename << "Model: " + completion.model + '\n';
    st_filename << sep_inner + '\n';
    st_filename << completion.prompt << '\n';
    st_filename << sep_inner + '\n';
    st_filename << completion.completion << '\n';
    st_filename << sep_outer + "\n\n";

    st_filename.close();
}

void export_chat_completion_response(const models::Completion &completion)
{
    fmt::print(fg(white), "Export:\n");
    std::string choice;

    while (true) {
        std::cout << "> Write reply to file? [y/n]: ";
        std::cin >> choice;

        if (choice == "y" or choice == "n") {
            break;
        } else {
            std::cout << "> Invalid choice. Input either 'y' or 'n'!\n";
        }
    }

    if (choice == "n") {
        std::cout << "> Not exporting response.\n";
        return;
    }

    write_message_to_file(completion);
}

} // namespace

void command_run(int argc, char **argv)
{
    ParamsRun params = cli::get_opts_run(argc, argv);
    params.sanitize();
    print_sep();

    static std::filesystem::path inputfile = std::filesystem::current_path() / "Inputfile";
    std::string prompt;

    if (params.prompt.has_value()) {
        prompt = params.prompt.value();
    } else {
        if (params.prompt_file.has_value()) {
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

    const models::Completion completion = run_query(model, prompt, temperature);
    print_sep();

    if (params.json_dump_file.has_value()) {
        dump_chat_completion_response(completion, params.json_dump_file.value());
        return;
    }

    print_usage_statistics(completion);
    print_sep();

    print_chat_completion_response(completion.completion);
    print_sep();

    if (params.enable_export) {
        export_chat_completion_response(completion);
        print_sep();
    }
}
