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
void query_embeddings_api(const std::string &post_fields, std::string &response);
