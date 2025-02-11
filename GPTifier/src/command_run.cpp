#include "command_run.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "datadir.hpp"
#include "input_selection.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "utils.hpp"

#include <chrono>
#include <ctime>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <thread>

using json = nlohmann::json;

namespace {

struct Completion {
    std::string content;
    std::string model;
    std::string prompt;
    std::time_t created = 0;
};

std::string select_chat_model()
{
#ifdef TESTING_ENABLED
    static std::string low_cost_model = "gpt-3.5-turbo";
    return low_cost_model;
#endif

    if (configs.chat.model.has_value()) {
        return configs.chat.model.value();
    }

    throw std::runtime_error("No model provided via configuration file or command line");
}

std::string create_chat_completion(const std::string &model, const std::string &prompt, float temperature)
{
    const json messages = { { "role", "user" }, { "content", prompt } };
    const json data = {
        { "model", model }, { "temperature", temperature }, { "messages", json::array({ messages }) }
    };

    Curl curl;
    return curl.create_chat_completion(data.dump());
}

void print_chat_completion_response(const json &results)
{
    const std::string content_original = results["choices"][0]["message"]["content"];

    json results_copy = results;
    results_copy["choices"][0]["message"]["content"] = "...";

    fmt::print(fg(white), "Response: ");
    fmt::print("{}\n", results_copy.dump(4));
    print_sep();

    fmt::print(fg(white), "Results: ");
    fmt::print(fg(green), "{}\n", content_original);

    print_sep();
}

void write_message_to_file(const Completion &completion)
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
    st_filename << completion.content << '\n';
    st_filename << sep_outer + "\n\n";

    st_filename.close();
}

void export_chat_completion_response(const json &results, const std::string &prompt)
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
        print_sep();
        return;
    }

    Completion completion;

    try {
        completion = {
            results["choices"][0]["message"]["content"],
            results["model"],
            prompt,
            results["created"],
        };
    } catch (const json::type_error &e) {
        const std::string errmsg = fmt::format("Failed to parse completion. Error was:\n{}", e.what());
        throw std::runtime_error(errmsg);
    }

    write_message_to_file(completion);
    print_sep();
}

void dump_chat_completion_response(const json &results, const std::string &json_dump_file)
{
    fmt::print("Dumping results to '{}'\n", json_dump_file);
    std::ofstream st_filename(json_dump_file);

    if (not st_filename.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'\n", json_dump_file);
        throw std::runtime_error(errmsg);
    }

    st_filename << std::setw(2) << results;
    st_filename.close();
}

bool timer_enabled = false;

void time_api_call()
{
    auto delay = std::chrono::milliseconds(250);
    auto start = std::chrono::high_resolution_clock::now();

    while (timer_enabled) {
        std::this_thread::sleep_for(delay);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        fmt::print(fg(white), "Time (s): ");
        fmt::print("{}\r", duration.count());
        std::cout.flush();
    }

    std::cout << "\n";
    print_sep();
}

} // namespace

void command_run(int argc, char **argv)
{
    ParamsRun params = cli::get_opts_run(argc, argv);
    params.sanitize();

    if (not params.prompt.has_value()) {
        print_sep();
        params.prompt = load_input_text(params.prompt_file);
    }

    print_sep();
    std::string model;

    if (params.model.has_value()) {
        model = params.model.value();
    } else {
        model = select_chat_model();
    }

    float temp = 1.00;
    if (std::holds_alternative<float>(params.temperature)) {
        temp = std::get<float>(params.temperature);
    }

    timer_enabled = true;
    std::thread timer(time_api_call);

    bool query_failed = false;
    std::string response;

    try {
        response = create_chat_completion(model, params.prompt.value(), temp);
    } catch (std::runtime_error &e) {
        query_failed = true;
        fmt::print(stderr, "{}\n", e.what());
    }

    timer_enabled = false;
    timer.join();

    if (query_failed) {
        throw std::runtime_error("Cannot proceed");
    }

    const json results = parse_response(response);

    if (params.json_dump_file.has_value()) {
        dump_chat_completion_response(results, params.json_dump_file.value());
        return;
    }

    print_chat_completion_response(results);

    if (params.enable_export) {
        export_chat_completion_response(results, params.prompt.value());
    }
}
