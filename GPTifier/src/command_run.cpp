#include "command_run.hpp"

#include "api.hpp"
#include "configs.hpp"
#include "help_messages.hpp"
#include "utils.hpp"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <curl/curl.h>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <thread>

struct RunParameters
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

void read_cli_run(const int argc, char **argv, RunParameters &params)
{
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
        int c = ::getopt_long(argc, argv, "hud:m:p:r:t:", long_options, &option_index);

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
            params.json_dump_file = ::optarg;
            break;
        case 'p':
            params.prompt = ::optarg;
            break;
        case 't':
            params.temperature = ::optarg;
            break;
        case 'r':
            params.prompt_file = ::optarg;
            break;
        case 'm':
            params.model = ::optarg;
            break;
        default:
            std::cerr << "Try running with -h or --help for more information\n";
            ::exit(EXIT_FAILURE);
        }
    };
}

void get_prompt(RunParameters &params)
{
    // Prompt was passed via command line
    if (not params.prompt.empty())
    {
        return;
    }

    ::print_separator();

    // Prompt was passed via file
    if (not params.prompt_file.empty())
    {
        ::read_text_from_file(params.prompt_file, params.prompt);
        return;
    }

    // Otherwise default to reading from stdin
    std::cout << "\033[1mInput:\033[0m ";
    std::getline(std::cin, params.prompt);

    // If still empty then we cannot proceed
    if (params.prompt.empty())
    {
        throw std::runtime_error("Prompt cannot be empty");
    }
}

void select_chat_model(nlohmann::json &body, const std::string &model)
{
    // I.e. model was passed via command line option
    if (not model.empty())
    {
        body["model"] = model;
        return;
    }

    // I.e. default to using low cost model since we are running unit tests
    const char *pytest_current_test = std::getenv("PYTEST_CURRENT_TEST");

    if (pytest_current_test)
    {
        std::cout << "Detected pytest run (" << pytest_current_test << ")\n";
        static std::string low_cost_model = "gpt-3.5-turbo";

        std::cout << "Defaulting to using a low cost model: " << low_cost_model << '\n';
        body["model"] = low_cost_model;

        return;
    }

    // I.e. load default model from configuration file
    if (not ::configs.chat.model.empty())
    {
        body["model"] = ::configs.chat.model;
        return;
    }

    throw std::runtime_error("No model provided via configuration file or command line");
}

void get_post_fields(std::string &post_fields, const RunParameters &params)
{
    nlohmann::json body = {};
    ::select_chat_model(body, params.model);

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
    post_fields = body.dump(2);

    ::print_separator();
    std::cout << "\033[1mRequest:\033[0m " + post_fields + '\n';
    ::print_separator();
}

bool RUN_TIMER = false;

void time_api_call()
{
    auto delay = std::chrono::milliseconds(250);
    auto start = std::chrono::high_resolution_clock::now();

    while (::RUN_TIMER)
    {
        std::this_thread::sleep_for(delay);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        std::cout << "\033[1mTime (s):\033[0m " << duration.count() << "\r";
        std::cout.flush();
    }

    std::cout << "\n";
    ::print_separator();
}

void query_chat_completion_api(::CURL *curl, const std::string &post_fields, std::string &response)
{
    static std::string url_chat_completions = "https://api.openai.com/v1/chat/completions";
    ::curl_easy_setopt(curl, ::CURLOPT_URL, url_chat_completions.c_str());

    ::curl_easy_setopt(curl, ::CURLOPT_POST, 1L);
    ::curl_easy_setopt(curl, ::CURLOPT_POSTFIELDS, post_fields.c_str());

    ::curl_easy_setopt(curl, ::CURLOPT_WRITEDATA, &response);

    ::RUN_TIMER = true;
    std::thread timer(::time_api_call);

    ::CURLcode rv = ::curl_easy_perform(curl);

    ::RUN_TIMER = false;
    timer.join();

    if (rv != ::CURLE_OK)
    {
        std::string errmsg = "Failed to run query. " + std::string(::curl_easy_strerror(rv));
        throw std::runtime_error(errmsg);
    }
}

void print_chat_completion_response(const std::string &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error"))
    {
        std::string error = results["error"]["message"];
        results["error"]["message"] = "<See Results section>";

        std::cout << "\033[1mResponse:\033[0m " + results.dump(2) + "\n";
        ::print_separator();

        std::cout << "\033[1mResults:\033[31m " + error + "\033[0m\n";
    }
    else
    {
        std::string content = results["choices"][0]["message"]["content"];
        results["choices"][0]["message"]["content"] = "<See Results section>";

        std::cout << "\033[1mResponse:\033[0m " + results.dump(2) + "\n";
        ::print_separator();

        std::cout << "\033[1mResults:\033[32m " + content + "\033[0m\n";
    }

    ::print_separator();
}

void write_message_to_file(const Completion &completion)
{
    std::string path_completion = ::get_proj_home_dir() + "/completions.gpt";

    std::cout << "> Writing reply to file " + path_completion + '\n';

    std::ofstream st_filename(path_completion, std::ios::app);
    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open " + path_completion);
    }

    std::string created = ::datetime_from_unix_timestamp(completion.created);

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
        ::print_separator();
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
        ::print_separator();
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

    ::write_message_to_file(completion);
    ::print_separator();
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
    static short int indent_pretty_print = 2;

    st_filename << std::setw(indent_pretty_print) << results;
    st_filename.close();
}

void command_run(const int argc, char **argv)
{
    RunParameters run_parameters;
    ::read_cli_run(argc, argv, run_parameters);

    if (run_parameters.print_help)
    {
        help::command_run();
        return;
    }

    ::get_prompt(run_parameters);

    std::string post_fields;
    ::get_post_fields(post_fields, run_parameters);

    Curl curl;
    std::string response;
    ::query_chat_completion_api(curl.handle, post_fields, response);

    if (run_parameters.json_dump_file.empty())
    {
        ::print_chat_completion_response(response);
        if (run_parameters.enable_export)
        {
            ::export_chat_completion_response(response, run_parameters.prompt);
        }
    }
    else
    {
        ::dump_chat_completion_response(response, run_parameters.json_dump_file);
    }
}
