#include "api.hpp"
#include "configs.hpp"

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

    this->handle = ::curl_easy_init();

    if (this->handle == NULL)
    {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    std::string header_auth = "Authorization: Bearer " + ::configs.api_key;
    this->headers = ::curl_slist_append(this->headers, header_auth.c_str());
    this->headers = ::curl_slist_append(this->headers, "Content-Type: application/json");
    ::curl_easy_setopt(this->handle, ::CURLOPT_HTTPHEADER, this->headers);

    ::curl_easy_setopt(this->handle, ::CURLOPT_WRITEFUNCTION, ::write_callback);
}

Curl::~Curl()
{
    if (this->handle)
    {
        ::curl_slist_free_all(this->headers);
        ::curl_easy_cleanup(this->handle);
    }

    ::curl_global_cleanup();
}
