#pragma once

#include <curl/curl.h>
#include <expected>
#include <stdexcept>
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

struct Ok {
    long code = -1;
    std::string response;
};

struct Err {
    long code = -1;
    std::string response;
};

using CurlResult = std::expected<Ok, Err>;

inline CurlResult check_curl_code(CURL *handle, const CURLcode code, const std::string &response)
{
    if (code != CURLE_OK) {
        // rare but catch truly exceptional cases
        throw std::runtime_error(curl_easy_strerror(code));
    }

    long http_status_code = -1;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_status_code);

    if (http_status_code != 200) {
        return std::unexpected(Err { http_status_code, response });
    }

    return Ok { http_status_code, response };
}

} // namespace networking
