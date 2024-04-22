#include "command_run.hpp"

#include "api.hpp"
#include "params.hpp"
#include "prompts.hpp"
#include "responses.hpp"
#include "utils.hpp"

#include <chrono>
#include <curl/curl.h>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <thread>

bool run_timer = false;

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

std::string create_chat_completion(::CURL *curl, const std::string &post_fields)
{
    static std::string url_chat_completions = "https://api.openai.com/v1/chat/completions";
    ::curl_easy_setopt(curl, ::CURLOPT_URL, url_chat_completions.c_str());

    ::curl_easy_setopt(curl, ::CURLOPT_POST, 1L);
    ::curl_easy_setopt(curl, ::CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
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

    return response;
}

void command_run()
{
    prompt::get_prompt();

    std::string post_fields;
    ::get_post_fields(post_fields);

    ::log_post_fields(post_fields);

    Curl curl;
    std::string response = ::create_chat_completion(curl.handle, post_fields);

    if (::params.dump.empty())
    {
        responses::print_chat_completion_response(response);

        if (::params.enable_export)
        {
            responses::export_chat_completion_response(response);
        }
    }
    else
    {
        responses::dump_chat_completion_response(response);
    }
}
