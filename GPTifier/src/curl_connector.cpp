#include "curl_connector.hpp"

#include <stdexcept>
#include <string>

namespace {

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

} // namespace

Curl::Curl()
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

Curl::~Curl()
{
    if (this->handle) {
        curl_slist_free_all(this->headers);
        curl_easy_cleanup(this->handle);
    }

    curl_global_cleanup();
}

void catch_curl_error(CURLcode code)
{
    if (code != CURLE_OK) {
        const std::string errmsg = "Failed to run query. " + std::string(curl_easy_strerror(code));
        throw std::runtime_error(errmsg);
    }
}
