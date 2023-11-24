#pragma once

#include <curl/curl.h>
#include <string>

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data);

class QueryHandler
{
private:
    void time_query();

    ::CURL *curl = NULL;

    std::string payload;
    std::string response;
    bool run_timer = false;

public:
    QueryHandler();
    ~QueryHandler();

    void build_payload(const std::string &prompt, const std::string &model, const std::string &temp);
    void print_payload();
    void run_query(const std::string &api_key);
    void print_response();
    void dump_response(const std::string &filename);
};
