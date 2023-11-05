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

    public:
        QueryHandler(const Configs &configs);
        ~QueryHandler();
        void run_query(const std::string &prompt, std::string &reply);
};
