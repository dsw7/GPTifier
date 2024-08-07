#include "command_models.hpp"

#include "api.hpp"
#include "help_messages.hpp"
#include "utils.hpp"

#include <curl/curl.h>
#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>

bool should_print_help = false;

void read_cli_models(const int argc, char **argv)
{
    while (true)
    {
        static struct option long_options[] = {{"help", no_argument, 0, 'h'}, {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "h", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            ::should_print_help = true;
            break;
        default:
            std::cerr << "Try running with -h or --help for more information\n";
            ::exit(EXIT_FAILURE);
        }
    }
}

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

void print_row(const std::string &id, const std::string &owned_by, const std::string &creation_time)
{
    std::cout << fmt::format("{:<30}{:<30}{}\n", id, owned_by, creation_time);
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

    ::print_separator();
    ::print_row("Model ID", "Owner", "Creation time");
    ::print_separator();

    for (const auto &entry : results["data"])
    {
        std::string id = entry["id"];
        std::string owned_by = entry["owned_by"];
        std::string creation_time = ::datetime_from_unix_timestamp(entry["created"]);
        ::print_row(id, owned_by, creation_time);
    }

    ::print_separator();
}

void command_models(const int argc, char **argv)
{
    ::read_cli_models(argc, argv);

    if (::should_print_help)
    {
        help::command_models();
        return;
    }

    Curl curl;
    std::string response;

    ::query_models_api(curl.handle, response);
    ::print_models_response(response);
}
