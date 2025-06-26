#pragma once

#include <curl/curl.h>
#include <string>

namespace networking {

class Curl {
public:
    Curl();
    ~Curl();

    CURL *get_handle();
    curl_slist *get_headers();
    void append_header(const std::string &header);

    // We want to prevent any copies from being made otherwise we'll attempt
    // to delete a shallow copy of the headers list multiple times (i.e. because the destructor will
    // be called for each copy)
    Curl(const Curl &) = delete;
    Curl &operator=(const Curl &) = delete;

private:
    CURL *curl_ = nullptr;
    curl_slist *headers_ = nullptr;
};

} // namespace networking
