#pragma once

#include "requests.h"

#include <curl/curl.h>
#include <string>

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data);

class QueryHandler
{
private:
    void time_query();

    ::CURL *curl = NULL;

    std::string response;
    bool run_timer = false;

public:
    QueryHandler();
    ~QueryHandler();

    void run_query(const std::string &api_key, const ::req_str &request);
    void print_response();
    void dump_response(const std::string &filename);
};
