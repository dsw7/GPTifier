#include "run_completion.hpp"
#include "params.hpp"
#include "utils.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

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

std::string create_chat_completion(::CURL *curl, const std::string &request)
{
    static std::string url_chat_completions = "https://api.openai.com/v1/chat/completions";
    ::curl_easy_setopt(curl, ::CURLOPT_URL, url_chat_completions.c_str());

    ::curl_easy_setopt(curl, ::CURLOPT_POST, 1L);
    ::curl_easy_setopt(curl, ::CURLOPT_POSTFIELDS, request.c_str());

    std::string response;
    ::curl_easy_setopt(curl, ::CURLOPT_WRITEDATA, &response);

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
