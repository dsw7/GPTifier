#include "curl_base.hpp"

#include <stdexcept>
#include <string>

namespace {

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

} // namespace

CurlBase::CurlBase()
{
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    this->handle = curl_easy_init();

    if (this->handle == NULL) {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    curl_easy_setopt(this->handle, CURLOPT_WRITEFUNCTION, write_callback);
}

CurlBase::~CurlBase()
{
    if (this->handle) {
        curl_slist_free_all(this->headers);
        curl_easy_cleanup(this->handle);
    }

    curl_global_cleanup();
}

void CurlBase::set_content_type_submit_form()
{
    const std::string header = "Content-Type: multipart/form-data";
    this->headers = curl_slist_append(this->headers, header.c_str());
}

void CurlBase::set_content_type_transmit_json()
{
    const std::string header = "Content-Type: application/json";
    this->headers = curl_slist_append(this->headers, header.c_str());
}

void catch_curl_error(CURLcode code)
{
    if (code != CURLE_OK) {
        const std::string errmsg = "Failed to run query. " + std::string(curl_easy_strerror(code));
        throw std::runtime_error(errmsg);
    }
}
