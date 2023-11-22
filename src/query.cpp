#include "query.h"
#include "utils.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <thread>

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

QueryHandler::QueryHandler(const Configs &configs)
{
    this->configs = configs;
    this->run_timer = true;

    if (::curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
    {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    this->curl = ::curl_easy_init();

    if (this->curl == NULL)
    {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }
}

QueryHandler::~QueryHandler()
{
    this->run_timer = false;

    if (this->curl)
    {
        ::curl_easy_cleanup(curl);
    }
    ::curl_global_cleanup();
}

void QueryHandler::build_payload(const std::string &prompt)
{
    nlohmann::json body = {};
    body["model"] = this->configs.model;

    nlohmann::json messages = {};
    messages["role"] = "user";
    messages["content"] = prompt;

    body["messages"] = nlohmann::json::array({messages});

    this->payload = body.dump(2);
}

void QueryHandler::print_payload()
{
    utils::print_separator();

    if (this->payload.empty())
    {
        throw std::runtime_error("Payload is empty. Will not print payload");
    }

    std::cout << "\033[1mPayload:\033[0m " + payload + '\n';

    utils::print_separator();
}

void QueryHandler::time_query()
{
    auto delay = std::chrono::milliseconds(250);
    auto start = std::chrono::high_resolution_clock::now();

    while (this->run_timer)
    {
        std::this_thread::sleep_for(delay);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        std::cout << "\033[1mTime (s):\033[0m " << duration.count() << "\r";
        std::cout.flush();
    }

    std::cout << "\n";
    utils::print_separator();
}

void QueryHandler::run_query()
{
    if (this->payload.empty())
    {
        throw std::runtime_error("Payload is empty. Cannot run query");
    }

    ::curl_easy_setopt(this->curl, ::CURLOPT_POSTFIELDS, this->payload.c_str());

    static std::string url = "https://api.openai.com/v1/chat/completions";
    ::curl_easy_setopt(this->curl, ::CURLOPT_URL, url.c_str());

    struct ::curl_slist *headers = NULL;
    headers = ::curl_slist_append(headers, "Content-Type: application/json");

    std::string header_auth = "Authorization: Bearer " + this->configs.api_key;
    headers = ::curl_slist_append(headers, header_auth.c_str());

    ::curl_easy_setopt(this->curl, ::CURLOPT_HTTPHEADER, headers);
    ::curl_easy_setopt(this->curl, ::CURLOPT_WRITEFUNCTION, ::write_callback);
    ::curl_easy_setopt(this->curl, ::CURLOPT_WRITEDATA, &this->response);

    std::thread timer(&QueryHandler::time_query, this);

    ::CURLcode rv = ::curl_easy_perform(this->curl);

    this->run_timer = false;
    timer.join();

    if (rv != ::CURLE_OK)
    {
        throw std::runtime_error("Failed to run query");
    }
}

void QueryHandler::print_response()
{
    if (this->response.empty())
    {
        throw std::runtime_error("Response is empty. Cannot print response");
    }

    nlohmann::json results = nlohmann::json::parse(this->response);

    if (results.contains("error"))
    {
        std::string error = results["error"]["message"];
        results["error"]["message"] = "<See Results section>";

        std::cout << "\033[1mResponse:\033[0m " + results.dump(2) + "\n";
        utils::print_separator();

        std::cout << "\033[1mResults:\033[31m " + error + "\033[0m\n";
    }
    else
    {
        std::string content = results["choices"][0]["message"]["content"];
        results["choices"][0]["message"]["content"] = "<See Results section>";

        std::cout << "\033[1mResponse:\033[0m " + results.dump(2) + "\n";
        utils::print_separator();

        std::cout << "\033[1mResults:\033[32m " + content + "\033[0m\n";
    }

    utils::print_separator();
}

void QueryHandler::dump_response(const std::string &filename)
{
    if (this->response.empty())
    {
        throw std::runtime_error("Response is empty. Cannot dump response to file");
    }

    std::cout << "Dumping results to " + filename + '\n';
    std::ofstream st_filename(filename);

    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open " + filename);
    }

    nlohmann::json results = nlohmann::json::parse(this->response);
    static short int indent_pretty_print = 2;

    st_filename << std::setw(indent_pretty_print) << results << std::endl;
    st_filename.close();
}
