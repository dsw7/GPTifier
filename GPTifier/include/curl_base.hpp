#pragma once

#include <curl/curl.h>

class CurlBase {
public:
    CurlBase();
    ~CurlBase();

    // We want to prevent any copies from being made otherwise we'll attempt
    // to delete a shallow copy of the headers list multiple times (i.e. because the destructor will
    // be called for each copy)
    CurlBase(const CurlBase &) = delete;
    CurlBase &operator=(const CurlBase &) = delete;

protected:
    CURL *handle = NULL;
    struct curl_slist *headers = NULL;

    void set_writefunction();
    void set_content_type_submit_form();
    void set_content_type_transmit_json();
    void run_easy_perform();
};

void catch_curl_error(CURLcode code);
