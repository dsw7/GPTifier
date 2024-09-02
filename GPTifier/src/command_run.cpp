#include "command_run.hpp"

#include "api.hpp"
#include "configs.hpp"
#include "help_messages.hpp"
#include "input_selection.hpp"
#include "reporting.hpp"
#include "testing.hpp"
#include "utils.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <thread>

namespace
{

struct ParamsRun
{
    bool enable_export = true;
    bool print_help = false;
    std::string json_dump_file;
    std::string model;
    std::string prompt;
    std::string prompt_file;
    std::string temperature = "1";
};

struct Completion
{
    std::string content;
    std::string model;
    std::string prompt;
    std::time_t created = 0;
};

ParamsRun read_cli_run(const int argc, char **argv)
{
    ParamsRun params;
    while (true)
    {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"no-interactive-export", no_argument, 0, 'u'},
            {"dump", required_argument, 0, 'd'},
            {"model", required_argument, 0, 'm'},
            {"prompt", required_argument, 0, 'p'},
            {"read-from-file", required_argument, 0, 'r'},
            {"temperature", required_argument, 0, 't'},
            {0, 0, 0, 0},
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hud:m:p:r:t:", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            params.print_help = true;
            break;
        case 'u':
            params.enable_export = false;
            break;
        case 'd':
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
            std::cerr << "Try running with -h or --help for more information\n";
            exit(EXIT_FAILURE);
        }
    };

    return params;
}

nlohmann::json select_chat_model(const std::string &model)
{
    nlohmann::json body = {};

    // I.e. model was passed via command line option
    if (not model.empty())
    {
        body["model"] = model;
        return body;
    }

    // I.e. default to using low cost model since we are running unit tests
    if (::is_test_running())
    {
        static std::string low_cost_model = "gpt-3.5-turbo";
        std::cout << "Defaulting to using a low cost model: " << low_cost_model << '\n';

        body["model"] = low_cost_model;
        return body;
    }

    // I.e. load default model from configuration file
    if (not configs.chat.model.empty())
    {
        body["model"] = configs.chat.model;
        return body;
    }

    throw std::runtime_error("No model provided via configuration file or command line");
}

std::string get_post_fields(const ParamsRun &params)
{
    nlohmann::json body = select_chat_model(params.model);

    try
    {
        body["temperature"] = std::stof(params.temperature);
    }
    catch (std::invalid_argument &e)
    {
        std::string errmsg = std::string(e.what()) + ". Failed to convert '" + params.temperature + "' to float";
        throw std::runtime_error(errmsg);
    }

    nlohmann::json messages = {};
    messages["role"] = "user";
    messages["content"] = params.prompt;

    body["messages"] = nlohmann::json::array({messages});
    std::string post_fields = body.dump(2);

    Reporting::print_sep();
    Reporting::print_request(post_fields);
    Reporting::print_sep();

    return post_fields;
}

void print_chat_completion_response(const std::string &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error"))
    {
        Reporting::print_response(results.dump(2));
        Reporting::print_sep();

        std::string error = results["error"]["message"];
        Reporting::print_error(error);
    }
    else
    {
        std::string content = results["choices"][0]["message"]["content"];
        results["choices"][0]["message"]["content"] = "...";

        Reporting::print_response(results.dump(2));
        Reporting::print_sep();
        Reporting::print_results(content);
    }

    Reporting::print_sep();
}

void write_message_to_file(const Completion &completion)
{
    std::string path_completion = get_proj_home_dir() + "/completions.gpt";

    std::cout << "> Writing reply to file " + path_completion + '\n';

    std::ofstream st_filename(path_completion, std::ios::app);
    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open " + path_completion);
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
        Reporting::print_sep();
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
        Reporting::print_sep();
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
    Reporting::print_sep();
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
    Reporting::print_sep();
}

} // namespace

void command_run(const int argc, char **argv)
{
    ParamsRun params = read_cli_run(argc, argv);

    if (params.print_help)
    {
        help::command_run();
        return;
    }

    if (params.prompt.empty())
    {
        Reporting::print_sep();
        params.prompt = load_input_text(params.prompt_file);
    }

    std::string post_fields = get_post_fields(params);

    timer_enabled = true;
    std::thread timer(time_api_call);

    bool query_failed = false;
    std::string response;

    try
    {
        response = query_chat_completion_api(post_fields);
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
