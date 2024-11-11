#include "command_run.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "datadir.hpp"
#include "input_selection.hpp"
#include "json.hpp"
#include "reporting.hpp"
#include "request_bodies.hpp"
#include "testing.hpp"
#include "utils.hpp"

#include <chrono>
#include <ctime>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

namespace
{

struct Completion
{
    std::string content;
    std::string model;
    std::string prompt;
    std::time_t created = 0;
};

std::string select_chat_model()
{
    // I.e. default to using low cost model since we are running unit tests
    if (testing::is_test_running())
    {
        static std::string low_cost_model = "gpt-3.5-turbo";
        std::cout << "Defaulting to using a low cost model: " << low_cost_model << '\n';

        return low_cost_model;
    }

    // I.e. try to load default model from configuration file
    if (configs.chat.model.empty())
    {
        throw std::runtime_error("No model provided via configuration file or command line");
    }

    return configs.chat.model;
}

std::string build_chat_completion_request_body(const cli::ParamsRun &params)
{
    float temperature = 1.00;

    try
    {
        temperature = std::stof(params.temperature);
    }
    catch (std::invalid_argument &e)
    {
        std::string errmsg = fmt::format("{}\nFailed to convert '{}' to float", e.what(), params.temperature);
        throw std::runtime_error(errmsg);
    }

    std::string model;

    if (params.model.empty())
    {
        model = select_chat_model();
    }
    else
    {
        // Model was passed via CLI
        model = params.model;
    }

    std::string request_body = get_chat_completion_request_body(model, params.prompt, temperature);

    reporting::print_sep();
    reporting::print_request(request_body);
    reporting::print_sep();

    return request_body;
}

void print_chat_completion_response(const std::string &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error"))
    {
        reporting::print_response(results.dump(2));
        reporting::print_sep();

        std::string error = results["error"]["message"];
        reporting::print_error(error);
    }
    else
    {
        std::string content = results["choices"][0]["message"]["content"];
        results["choices"][0]["message"]["content"] = "...";

        reporting::print_response(results.dump(2));
        reporting::print_sep();
        reporting::print_results(content);
    }

    reporting::print_sep();
}

void write_message_to_file(const Completion &completion)
{
    std::cout << fmt::format("> Writing completion to file {}\n", datadir::GPT_COMPLETIONS);

    std::ofstream st_filename(datadir::GPT_COMPLETIONS, std::ios::app);
    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open " + datadir::GPT_COMPLETIONS);
    }

    std::string created = datetime_from_unix_timestamp(completion.created);

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

void export_chat_completion_response(const std::string &response, const std::string &prompt)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error"))
    {
        std::cerr << "Cannot export results as error occurred\n";
        reporting::print_sep();
        return;
    }

    std::cout << "\033[1mExport:\033[0m\n";
    std::string choice;

    while (true)
    {
        std::cout << "> Write reply to file? [y/n]: ";
        std::cin >> choice;

        if (choice.compare("y") == 0 or choice.compare("n") == 0)
        {
            break;
        }
        else
        {
            std::cout << "> Invalid choice. Input either 'y' or 'n'!\n";
        }
    }

    if (choice.compare("n") == 0)
    {
        std::cout << "> Not exporting response.\n";
        reporting::print_sep();
        return;
    }

    Completion completion;
    completion.prompt = prompt;

    try
    {
        completion.content = results["choices"][0]["message"]["content"];
        completion.created = results["created"];
        completion.model = results["model"];
    }
    catch (const nlohmann::json::type_error &e)
    {
        std::string errmsg = "Failed to parse completion. Error was: '" + std::string(e.what()) + "'";
        throw std::runtime_error(errmsg);
    }

    write_message_to_file(completion);
    reporting::print_sep();
}

void dump_chat_completion_response(const std::string &response, const std::string &json_dump_file)
{
    std::cout << "Dumping results to " + json_dump_file + '\n';
    std::ofstream st_filename(json_dump_file);

    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open '" + json_dump_file + "'");
    }

    nlohmann::json results = nlohmann::json::parse(response);

    st_filename << std::setw(2) << results;
    st_filename.close();
}

bool timer_enabled = false;

void time_api_call()
{
    auto delay = std::chrono::milliseconds(250);
    auto start = std::chrono::high_resolution_clock::now();

    while (timer_enabled)
    {
        std::this_thread::sleep_for(delay);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        std::cout << "\033[1mTime (s):\033[0m " << duration.count() << "\r";
        std::cout.flush();
    }

    std::cout << "\n";
    reporting::print_sep();
}

} // namespace

void command_run(const int argc, char **argv)
{
    cli::ParamsRun params = cli::get_opts_run(argc, argv);

    if (params.prompt.empty())
    {
        reporting::print_sep();
        params.prompt = load_input_text(params.prompt_file);
    }

    std::string request_body = build_chat_completion_request_body(params);

    timer_enabled = true;
    std::thread timer(time_api_call);

    bool query_failed = false;
    std::string response;

    try
    {
        response = query_chat_completion_api(request_body);
    }
    catch (std::runtime_error &e)
    {
        query_failed = true;
        std::cerr << e.what() << '\n';
    }

    timer_enabled = false;
    timer.join();

    if (query_failed)
    {
        throw std::runtime_error("Cannot proceed");
    }

    if (params.json_dump_file.empty())
    {
        print_chat_completion_response(response);
        if (params.enable_export)
        {
            export_chat_completion_response(response, params.prompt);
        }
    }
    else
    {
        dump_chat_completion_response(response, params.json_dump_file);
    }
}
