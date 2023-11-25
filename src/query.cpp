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

QueryHandler::QueryHandler()
{
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

::str_response QueryHandler::run_query(const std::string &api_key, const ::str_request &request)
{
    if (request.empty())
    {
        std::cerr << "Request is empty. Cannot run query\n";
        return std::string();
    }

    ::curl_easy_setopt(this->curl, ::CURLOPT_POSTFIELDS, request.c_str());

    static std::string url = "https://api.openai.com/v1/chat/completions";
    ::curl_easy_setopt(this->curl, ::CURLOPT_URL, url.c_str());

    struct ::curl_slist *headers = NULL;
    headers = ::curl_slist_append(headers, "Content-Type: application/json");

    std::string header_auth = "Authorization: Bearer " + api_key;
    headers = ::curl_slist_append(headers, header_auth.c_str());

    ::curl_easy_setopt(this->curl, ::CURLOPT_HTTPHEADER, headers);
    ::curl_easy_setopt(this->curl, ::CURLOPT_WRITEFUNCTION, ::write_callback);

    std::string response;
    ::curl_easy_setopt(this->curl, ::CURLOPT_WRITEDATA, &response);

    std::thread timer(&QueryHandler::time_query, this);

    ::CURLcode rv = ::curl_easy_perform(this->curl);

    this->run_timer = false;
    timer.join();

    if (rv != ::CURLE_OK)
    {
        std::cerr << "Failed to run query. " << ::curl_easy_strerror(rv) + '\n';
        return std::string();
    }

    return response;
}
