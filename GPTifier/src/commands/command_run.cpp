#include "command_run.hpp"

#include "configs.hpp"
#include "datadir.hpp"
#include "responses.hpp"
#include "utils.hpp"

#include <algorithm>
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

const float MIN_TEMP = 0.00;
const float MAX_TEMP = 2.00;

void help_run()
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
    std::optional<std::string> json_dump_file;
    std::optional<std::string> model;
    std::optional<std::string> prompt;
    std::optional<std::string> prompt_file;
    std::optional<std::string> temperature;
};

Parameters read_cli(int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "file", required_argument, 0, 'o' },
            { "model", required_argument, 0, 'm' },
            { "prompt", required_argument, 0, 'p' },
            { "read-from-file", required_argument, 0, 'r' },
            { "temperature", required_argument, 0, 't' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "ho:m:p:r:t:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_run();
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

std::string get_prompt(const Parameters &params)
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

float get_temperature(const std::optional<std::string> &temperature)
{
    float temp_f = 1.00;

    if (temperature) {
        if (temperature.value().empty()) {
            throw std::runtime_error("Empty temperature");
        }

        temp_f = utils::string_to_float(temperature.value());
    }

    return std::clamp(temp_f, MIN_TEMP, MAX_TEMP);
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

serialization::Response run_query(const std::string &model, const std::string &prompt, const float temperature)
{
    TIMER_ENABLED.store(true);
    std::thread timer(time_api_call);

    bool query_failed = false;
    serialization::ResponseResult result;
    std::mutex mutex_print_stderr;

    try {
        result = serialization::create_response(prompt, model, temperature);
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
        // handle any internal exceptions
        throw std::runtime_error("Cannot proceed");
    }

    if (not result.has_value()) {
        // handle exceptions coming from API (i.e. non 200 responses)
        throw std::runtime_error(fmt::format("{}\nCannot proceed", result.error().errmsg));
    }

    return result.value();
}

// Output ---------------------------------------------------------------------------------------------------

void dump_response(const serialization::Response &rp, const std::string &json_dump_file)
{
    const nlohmann::json json = {
        { "created", rp.created },
        { "id", rp.id },
        { "input", rp.input },
        { "input_tokens", rp.input_tokens },
        { "model", rp.model },
        { "output", rp.output },
        { "output_tokens", rp.output_tokens },
        { "rtt", rp.rtt.count() },
    };

    fmt::print("Dumping results to '{}'\n", json_dump_file);
    utils::write_to_file(json_dump_file, json.dump(2));
}

void print_response(const std::string &completion)
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

void print_usage_statistics(const serialization::Response &rp)
{
    const int wc_input = utils::get_word_count(rp.input);
    const int wc_output = utils::get_word_count(rp.output);

    fmt::print(fg(white), "Usage:\n");
    fmt::print("Model: {}\n", rp.model);
    fmt::print("RTT: {} s\n", rp.rtt.count());
    fmt::print("\n");

    fmt::print("Prompt tokens: ");
    fmt::print(fg(green), "{}\n", rp.input_tokens);
    fmt::print("Prompt size (words): ");
    fmt::print(fg(green), "{}\n", wc_input);
    print_ratio(rp.input_tokens, wc_input);
    fmt::print("\n");

    fmt::print("Completion tokens: ");
    fmt::print(fg(green), "{}\n", rp.output_tokens);
    fmt::print("Completion size (words): ");
    fmt::print(fg(green), "{}\n", wc_output);
    print_ratio(rp.output_tokens, wc_output);
}

void write_message_to_file(const serialization::Response &rp)
{
    const std::string created = utils::datetime_from_unix_timestamp(rp.created);

    std::string text = "{\n";
    text += "> Created at: " + created + " (GMT)\n";
    text += "> Model: " + rp.model + "\n\n";
    text += "> Input:\n" + rp.input + "\n\n";
    text += "> Output:\n" + rp.output + "\n";
    text += "}\n\n";

    const std::string path_completions_file = datadir::GPT_COMPLETIONS.string();
    fmt::print("> Writing output to file {}\n", path_completions_file);

    utils::append_to_file(path_completions_file, text);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

void export_response(const serialization::Response &rp)
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

    write_message_to_file(rp);
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

    const std::string prompt = get_prompt(params);

    if (prompt.empty()) {
        throw std::runtime_error("Prompt is empty");
    }

    const float temperature = get_temperature(params.temperature);
    const serialization::Response rp = run_query(model, prompt, temperature);

    if (params.json_dump_file) {
        dump_response(rp, params.json_dump_file.value());
        return;
    }

    print_usage_statistics(rp);
    utils::separator();

    print_response(rp.output);
    utils::separator();

#ifndef TESTING_ENABLED
    export_response(rp);
    utils::separator();
#endif
}

} // namespace commands
