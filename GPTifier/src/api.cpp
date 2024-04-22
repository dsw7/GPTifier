#include "api.hpp"
#include "params.hpp"

#include <stdexcept>

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

Curl::Curl()
{
    if (::curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
    {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    this->curl = ::curl_easy_init();

    if (this->curl == NULL)
    {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    std::string header_auth = "Authorization: Bearer " + ::params.api_key;
    this->headers = ::curl_slist_append(this->headers, header_auth.c_str());
    this->headers = ::curl_slist_append(this->headers, "Content-Type: application/json");
    ::curl_easy_setopt(this->curl, ::CURLOPT_HTTPHEADER, this->headers);

    ::curl_easy_setopt(this->curl, ::CURLOPT_WRITEFUNCTION, ::write_callback);
}

Curl::~Curl()
{
    if (this->curl)
    {
        // Leads to a Valgrind-detectable memory leak if headers are not freed
        ::curl_slist_free_all(this->headers);
        ::curl_easy_cleanup(this->curl);
    }

    ::curl_global_cleanup();
}
