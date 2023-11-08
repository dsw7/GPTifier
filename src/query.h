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
        std::string response;

    public:
        QueryHandler(const Configs &configs);
        ~QueryHandler();

        void build_payload(const std::string &prompt);
        void print_payload();
        void run_query();
        void print_response();
        void dump_response(const std::string &filename);
};
