#include "command_models.hpp"

#include "api.hpp"

#include <curl/curl.h>
#include <iostream>
#include <json.hpp>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

void query_models_api(::CURL *curl, std::string &response)
{
    static std::string url_get_models = "https://api.openai.com/v1/models";
    ::curl_easy_setopt(curl, ::CURLOPT_URL, url_get_models.c_str());

    ::curl_easy_setopt(curl, ::CURLOPT_HTTPGET, 1L);
    ::curl_easy_setopt(curl, ::CURLOPT_WRITEDATA, &response);

    ::CURLcode rv = ::curl_easy_perform(curl);
    if (rv != ::CURLE_OK)
    {
        std::string errmsg = "Failed to run query. " + std::string(::curl_easy_strerror(rv));
        throw std::runtime_error(errmsg);
    }
}

void print_models_response(const std::string &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error"))
    {
        std::string error = results["error"]["message"];
        std::cout << "\033[1mResults:\033[31m " + error + "\033[0m\n";
        return;
    }

    for (const auto &entry : results["data"])
    {
        std::cout << "> " << entry["id"] << "\n";
    }
}

void command_models()
{
    Curl curl;
    std::string response;

    ::query_models_api(curl.handle, response);
    ::print_models_response(response);
}
