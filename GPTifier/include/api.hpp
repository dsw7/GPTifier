#pragma once

#include <curl/curl.h>
#include <string>

class Curl
{
private:
    struct ::curl_slist *headers = NULL;

public:
    Curl();
    ~Curl();

    ::CURL *handle = NULL;
};

void query_models_api(std::string &response);
