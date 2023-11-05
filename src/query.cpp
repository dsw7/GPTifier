#include "query.h"

#include <stdexcept>
#include <nlohmann/json.hpp>
#include "presentation.h"

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

QueryHandler::QueryHandler(const Configs &configs)
{
    this->configs = configs;

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
    if (this->curl)
    {
        ::curl_easy_cleanup(curl);
    }

    ::curl_global_cleanup();
}

void QueryHandler::build_payload(const std::string &prompt)
{
    nlohmann::json body = {};
    body["model"] = "gpt-3.5-turbo";

    nlohmann::json messages = {};
    messages["role"] = "user";
    messages["content"] = prompt;

    body["messages"] = {messages};
    this->payload = body.dump(2);
    presentation::print_payload(this->payload);
}

void QueryHandler::run_query(std::string &reply)
{
    if (this->payload.empty())
    {
        throw std::runtime_error("Payload is empty. Cannot proceed");
    }
    ::curl_easy_setopt(this->curl, ::CURLOPT_POSTFIELDS, this->payload.c_str());

    static std::string url = "https://api.openai.com/v1/chat/completions";
    ::curl_easy_setopt(this->curl, ::CURLOPT_URL, url.c_str());

    struct ::curl_slist* headers = NULL;
    headers = ::curl_slist_append(headers, "Content-Type: application/json");

    std::string header_auth = "Authorization: Bearer " + this->configs.api_key;
    headers = ::curl_slist_append(headers, header_auth.c_str());

    ::curl_easy_setopt(this->curl, ::CURLOPT_HTTPHEADER, headers);
    ::curl_easy_setopt(this->curl, ::CURLOPT_WRITEFUNCTION, ::write_callback);
    ::curl_easy_setopt(this->curl, ::CURLOPT_WRITEDATA, &reply);

    if (::curl_easy_perform(this->curl) != ::CURLE_OK)
    {
        throw std::runtime_error("Failed to run query");
    }
}
