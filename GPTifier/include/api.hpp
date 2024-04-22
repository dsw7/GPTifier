#pragma once

#include <curl/curl.h>

class Curl
{
private:
    struct ::curl_slist *headers = NULL;

public:
    Curl();
    ~Curl();

    ::CURL *handle = NULL;
};
