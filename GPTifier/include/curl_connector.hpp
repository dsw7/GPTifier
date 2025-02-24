#pragma once

#include <curl/curl.h>

class Curl {
public:
    Curl();
    ~Curl();

    // We want to prevent any copies from being made otherwise we'll attempt
    // to delete a shallow copy of the headers list multiple times (i.e. because the destructor will
    // be called for each copy)
    Curl(const Curl &) = delete;
    Curl &operator=(const Curl &) = delete;

protected:
    CURL *handle = NULL;
    struct curl_slist *headers = NULL;
};

void catch_curl_error(CURLcode code);
