#include "query.h"

#include <curl/curl.h>
#include <iostream>
#include <stdexcept>

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

void run_query(const std::string &api_key, const std::string &prompt)
{
    static std::string model_id = "gpt-3.5-turbo";

    std::string header_content_type = "Content-Type: application/json";
    std::string header_auth = "Authorization: Bearer " + api_key;

    struct ::curl_slist* headers = NULL;

    headers = ::curl_slist_append(headers, header_content_type.c_str());
    if (headers == NULL)
    {
        throw std::runtime_error("Something went wrong when appending content-type header");
    }

    headers = ::curl_slist_append(headers, header_auth.c_str());
    if (headers == NULL)
    {
        throw std::runtime_error("Something went wrong when appending authorization header");
    }

    if (::curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
    {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    ::CURL* curl = ::curl_easy_init();

    if (curl == NULL)
    {
        ::curl_global_cleanup();
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    static std::string url = "https://api.openai.com/v1/chat/completions";
    ::curl_easy_setopt(curl, ::CURLOPT_URL, url.c_str());

    ::curl_easy_setopt(curl, ::CURLOPT_HTTPHEADER, headers);

    std::string data = "{\n\t\"model\": \"" + model_id + "\",\n\t\"messages\": [{ \"role\": \"system\", \"content\": \"You are a helpful assistant.\" }, { \"role\": \"user\", \"content\": \"" + prompt + "\" }]\n}";
    ::curl_easy_setopt(curl, ::CURLOPT_POSTFIELDS, data.c_str());

    std::string response;
    ::curl_easy_setopt(curl, ::CURLOPT_WRITEFUNCTION, ::write_callback);
    ::curl_easy_setopt(curl, ::CURLOPT_WRITEDATA, &response);

    ::CURLcode res = ::curl_easy_perform(curl);
    if (res != ::CURLE_OK)
    {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }
    else
    {
        std::cout << response << std::endl;
    }

    ::curl_easy_cleanup(curl);
    ::curl_global_cleanup();
}
