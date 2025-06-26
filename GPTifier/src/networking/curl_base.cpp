#include "curl_base.hpp"

#include <stdexcept>

namespace {

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

} // namespace

namespace networking {

Curl::Curl()
{
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    this->curl_ = curl_easy_init();

    if (this->curl_ == nullptr) {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    curl_easy_setopt(this->curl_, CURLOPT_WRITEFUNCTION, write_callback);
}

Curl::~Curl()
{
    if (this->curl_) {
        curl_slist_free_all(this->headers_);
        curl_easy_cleanup(this->curl_);
    }

    curl_global_cleanup();
}

CURL *Curl::get_handle()
{
    return this->curl_;
}

curl_slist *Curl::get_headers()
{
    return this->headers_;
}

} // namespace networking
