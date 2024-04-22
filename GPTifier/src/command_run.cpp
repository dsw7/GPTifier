#include "command_run.hpp"

#include "api.hpp"
#include "params.hpp"
#include "utils.hpp"

#include <chrono>
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

void read_prompt_from_file()
{
    ::print_separator();
    std::cout << "Reading prompt from file: " + ::params.prompt_file + '\n';

    std::ifstream file(::params.prompt_file);

    if (not file.is_open())
    {
        throw std::runtime_error("Could not open file '" + ::params.prompt_file + "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    ::params.prompt = buffer.str();

    file.close();
}

void read_prompt_interactively()
{
    ::print_separator();
    std::cout << "\033[1mInput:\033[0m ";

    std::getline(std::cin, ::params.prompt);
}

void get_prompt()
{
    // Prompt was passed via command line
    if (not ::params.prompt.empty())
    {
        return;
    }

    // Prompt was passed via file
    if (not ::params.prompt_file.empty())
    {
        ::read_prompt_from_file();
        return;
    }

    // Otherwise default to reading from stdin
    ::read_prompt_interactively();

    // If still empty then we cannot proceed
    if (::params.prompt.empty())
    {
        throw std::runtime_error("Prompt cannot be empty");
    }
}

void get_post_fields(std::string &post_fields)
{
    nlohmann::json body = {};

    body["model"] = ::params.model;

    try
    {
        body["temperature"] = std::stof(::params.temperature);
    }
    catch (std::invalid_argument &e)
    {
        std::string errmsg = std::string(e.what()) + ". Failed to convert '" + ::params.temperature + "' to float";
        throw std::runtime_error(errmsg);
    }

    nlohmann::json messages = {};
    messages["role"] = "user";
    messages["content"] = ::params.prompt;

    body["messages"] = nlohmann::json::array({messages});
    post_fields = body.dump(2);
}

void log_post_fields(const std::string &post_fields)
{
    ::print_separator();
    std::cout << "\033[1mRequest:\033[0m " + post_fields + '\n';
    ::print_separator();
}

bool run_timer = false;

void time_api_call()
{
    auto delay = std::chrono::milliseconds(250);
    auto start = std::chrono::high_resolution_clock::now();

    while (::run_timer)
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

    ::run_timer = true;
    std::thread timer(::time_api_call);

    ::CURLcode rv = ::curl_easy_perform(curl);

    ::run_timer = false;
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

void write_message_to_file(const std::string &message)
{
    std::string path_completion = ::get_proj_home_dir() + "/completions.gpt";

    std::cout << "> Writing reply to file " + path_completion + '\n';

    std::ofstream st_filename(path_completion, std::ios::app);
    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open " + path_completion);
    }

    std::string separator(110, '=');
    st_filename << separator + '\n';
    st_filename << "[GPTifier] Results:\n";
    st_filename << separator + '\n';
    st_filename << message << '\n';

    st_filename.close();
}

void export_chat_completion_response(const std::string &response)
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
    }
    else
    {
        ::write_message_to_file(results["choices"][0]["message"]["content"]);
    }

    ::print_separator();
}

void dump_chat_completion_response(const std::string &response)
{
    std::cout << "Dumping results to " + params.dump + '\n';
    std::ofstream st_filename(params.dump);

    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open '" + params.dump + "'");
    }

    nlohmann::json results = nlohmann::json::parse(response);
    static short int indent_pretty_print = 2;

    st_filename << std::setw(indent_pretty_print) << results;
    st_filename.close();
}

void command_run()
{
    ::get_prompt();

    std::string post_fields;

    ::get_post_fields(post_fields);
    ::log_post_fields(post_fields);

    Curl curl;
    std::string response;
    ::query_chat_completion_api(curl.handle, post_fields, response);

    if (::params.dump.empty())
    {
        ::print_chat_completion_response(response);
        if (::params.enable_export)
        {
            ::export_chat_completion_response(response);
        }
    }
    else
    {
        ::dump_chat_completion_response(response);
    }
}