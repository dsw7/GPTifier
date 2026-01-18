#include "command_run.hpp"

#include "configs.hpp"
#include "datadir.hpp"
#include "responses.hpp"
#include "utils.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

void help_run_command_()
{
    const std::string messages = R"(Create a response according to a prompt. A prompt can be provided
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
  -l, --use-local                Connect to locally hosted LLM as opposed to OpenAI
  -o, --file=FILE                Export results to a JSON file named FILE
  -p, --prompt=PROMPT            Provide prompt via command line
  -r, --read-from-file=FILENAME  Read prompt from a custom file named FILENAME
  -t, --temperature=TEMPERATURE  Provide a sampling temperature between 0 and 2. Note that
                                 temperature will be clamped between 0 and 2

Examples:
  > Run an interaction session:
    $ gpt run
  > Run a query non-interactively and export results
    $ gpt run --prompt="What is 3 + 5?" --file="/tmp/results.json"
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    bool use_local = false;
    std::optional<std::string> json_dump_file;
    std::optional<std::string> model;
    std::optional<std::string> prompt;
    std::optional<std::string> prompt_file;
    std::optional<std::string> temperature;
};

Parameters read_cli_(const int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "file", required_argument, 0, 'o' },
            { "model", required_argument, 0, 'm' },
            { "use-local", no_argument, 0, 'l' },
            { "prompt", required_argument, 0, 'p' },
            { "read-from-file", required_argument, 0, 'r' },
            { "temperature", required_argument, 0, 't' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "ho:m:lp:r:t:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_run_command_();
                exit(EXIT_SUCCESS);
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
            case 'l':
                params.use_local = true;
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

    if (params.temperature) {
        if (params.temperature.value().empty()) {
            throw std::runtime_error("Empty temperature");
        }
    }

    return params;
}

std::string get_prompt_(const Parameters &params)
{
    if (params.prompt) {
        return params.prompt.value();
    }

    if (params.prompt_file) {
        fmt::print("Reading text from file: '{}'\n", params.prompt_file.value());
        return utils::read_from_file(params.prompt_file.value());
    }

    static std::filesystem::path inputfile = std::filesystem::current_path() / "Inputfile";

    if (std::filesystem::exists(inputfile)) {
        fmt::print("Found an Inputfile in current working directory!\n");
        return utils::read_from_file(inputfile);
    }

    // Get prompt from stdin if all else fails
    fmt::print(fg(white), "Input: ");
    std::string prompt;
    std::getline(std::cin, prompt);
    utils::separator();

    return prompt;
}

// Completion -----------------------------------------------------------------------------------------------

using serialization::Response;

std::atomic<bool> TIMER_ENABLED(false);

void time_api_call_()
{
    const std::chrono::duration delay = std::chrono::milliseconds(100);

    static std::array spinner = { "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
    const int num_frames = spinner.size();

    while (TIMER_ENABLED.load()) {
        for (int i = 0; i < num_frames; ++i) {
            std::cout << "\r" << spinner[i] << std::flush;
            std::this_thread::sleep_for(delay);
        }
    }

    std::cout << " \r" << std::flush;
}

Response create_openai_response_(const std::string &model, const std::string &prompt, const float temperature)
{
    TIMER_ENABLED.store(true);
    std::thread timer(time_api_call_);

    bool query_failed = false;
    Response response;
    std::string errmsg;

    try {
        response = serialization::create_openai_response(prompt, model, temperature);
    } catch (std::runtime_error &e) {
        query_failed = true;
        errmsg = e.what();
    }

    TIMER_ENABLED.store(false);
    timer.join();

    if (query_failed) {
        fmt::print(stderr, "{}\n", errmsg);
        throw std::runtime_error("Cannot proceed");
    }

    return response;
}

Response create_ollama_response_(const std::string &model, const std::string &prompt)
{
    TIMER_ENABLED.store(true);
    std::thread timer(time_api_call_);

    bool query_failed = false;
    Response response;
    std::string errmsg;

    try {
        response = serialization::create_ollama_response(prompt, model);
    } catch (std::runtime_error &e) {
        query_failed = true;
        errmsg = e.what();
    }

    TIMER_ENABLED.store(false);
    timer.join();

    if (query_failed) {
        fmt::print(stderr, "{}\n", errmsg);
        throw std::runtime_error("Cannot proceed");
    }

    return response;
}

// Output ---------------------------------------------------------------------------------------------------

void dump_response_to_json_file_(const Response &response, const std::string &json_dump_file)
{
    const nlohmann::json json = {
        { "created", response.created },
        { "input", response.input },
        { "input_tokens", response.input_tokens },
        { "model", response.model },
        { "output", response.output },
        { "output_tokens", response.output_tokens },
        { "rtt", response.rtt.count() },
        { "source", response.source },
    };

    fmt::print("Dumping results to '{}'\n", json_dump_file);
    utils::write_to_file(json_dump_file, json.dump(2));
}

void print_completion_to_stdout_(const std::string &completion)
{
    fmt::print(fg(white), "Results: ");
    fmt::print(fg(green), "{}\n", completion);
}

void print_token_to_word_count_ratio_(int num_tokens, int num_words)
{
    if (num_words < 1) {
        throw std::runtime_error("Zero division. Number of words is 0!");
    }

    // The ideal ratio is about 100 tokens / 75 words
    // See https://platform.openai.com/tokenizer for more information
    const float ratio = (float)num_tokens / num_words;

    fmt::print("Ratio: ");

    if (ratio <= 2) {
        fmt::print(fg(green), "{}\n", ratio);
    } else if (ratio > 2 and ratio <= 3) {
        fmt::print(fg(yellow), "{}\n", ratio);
    } else {
        fmt::print(fg(red), "{}\n", ratio);
    }
}

void print_inference_usage_statistics_(const Response &response)
{
    const int wc_input = utils::get_word_count(response.input);
    const int wc_output = utils::get_word_count(response.output);

    fmt::print(fg(white), "Usage:\n");
    fmt::print("Model: {}\n", response.model);
    fmt::print("RTT: {} s\n", response.rtt.count());
    fmt::print("\n");

    fmt::print("Prompt tokens: ");
    fmt::print(fg(green), "{}\n", response.input_tokens);
    fmt::print("Prompt size (words): ");
    fmt::print(fg(green), "{}\n", wc_input);
    print_token_to_word_count_ratio_(response.input_tokens, wc_input);
    fmt::print("\n");

    fmt::print("Completion tokens: ");
    fmt::print(fg(green), "{}\n", response.output_tokens);
    fmt::print("Completion size (words): ");
    fmt::print(fg(green), "{}\n", wc_output);
    print_token_to_word_count_ratio_(response.output_tokens, wc_output);
}

void append_response_to_completions_file_(const Response &response)
{
    const std::string text = fmt::format(
        "{{\n"
        "> Created at: {} (GMT)\n"
        "> Source: {}\n\n"
        "> Model: {}\n\n"
        "> Input:\n{}\n\n"
        "> Output:\n{}\n"
        "}}\n\n",
        response.created, response.source, response.model, response.input, response.output);

    const std::string path_completions_file = datadir::GPT_COMPLETIONS.string();
    fmt::print("> Writing output to file {}\n", path_completions_file);

    utils::append_to_file(path_completions_file, text);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

void dump_response_to_completions_file_(const Response &response)
{
    fmt::print(fg(white), "Export:\n");
    char choice = 'n';

    while (true) {
        fmt::print("> Write reply to file? [y/n]: ");
        choice = std::cin.get();

        if (choice == 'y' or choice == 'n') {
            break;
        } else {
            fmt::print("> Invalid choice. Input either 'y' or 'n'!\n");
        }
    }

    if (choice == 'n') {
        fmt::print("> Not exporting response.\n");
        return;
    }

    append_response_to_completions_file_(response);
}

#pragma GCC diagnostic pop

void process_outgoing_response_(const Response &response)
{
    print_inference_usage_statistics_(response);
    utils::separator();

    print_completion_to_stdout_(response.output);
    utils::separator();

#ifndef TESTING_ENABLED
    dump_response_to_completions_file_(response);
    utils::separator();
#endif
}

// OpenAI / Ollama ------------------------------------------------------------------------------------------

void run_ollama_query_(const Parameters &params, const std::string &prompt)
{
    std::string model;

    if (params.model) {
        model = params.model.value();
    } else {
        model = configs.model_run_ollama.value();
    }

    if (model.empty()) {
        throw std::runtime_error("Model is empty");
    }

    const Response response = create_ollama_response_(model, prompt);

    if (params.json_dump_file) {
        dump_response_to_json_file_(response, params.json_dump_file.value());
    } else {
        process_outgoing_response_(response);
    }
}

void run_openai_query_(const Parameters &params, const std::string &prompt)
{
    std::string model;

    if (params.model) {
        model = params.model.value();
    } else {
#ifdef TESTING_ENABLED
        model = "gpt-3.5-turbo";
#else
        model = configs.model_run_openai.value();
#endif
    }

    if (model.empty()) {
        throw std::runtime_error("Model is empty");
    }

    const float temperature = utils::string_to_float(params.temperature.value_or("1.00"));
    const Response response = create_openai_response_(model, prompt, temperature);

    if (params.json_dump_file) {
        dump_response_to_json_file_(response, params.json_dump_file.value());
    } else {
        process_outgoing_response_(response);
    }
}

} // namespace

namespace commands {

void command_run(const int argc, char **argv)
{
    const Parameters params = read_cli_(argc, argv);

    utils::separator();
    const std::string prompt = get_prompt_(params);

    if (prompt.empty()) {
        throw std::runtime_error("Prompt is empty");
    }

    if (params.use_local) {
        run_ollama_query_(params, prompt);
    } else {
        run_openai_query_(params, prompt);
    }
}

} // namespace commands
