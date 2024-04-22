#pragma once

#include <curl/curl.h>
#include <string>

class QueryHandler
{
private:
    void time_query();
    ::CURL *curl = NULL;
    bool run_timer = false;

public:
    QueryHandler();
    ~QueryHandler();
    std::string run_query(const std::string &request);
};
