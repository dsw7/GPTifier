#pragma once

#include "configs.h"

#include <curl/curl.h>
#include <string>

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data);

class QueryHandler
{
    private:
        Configs configs;
        ::CURL* curl = NULL;
        std::string payload;

    public:
        QueryHandler(const Configs &configs);
        ~QueryHandler();

        void build_payload(const std::string &prompt);
        void run_query(std::string &reply);
};
