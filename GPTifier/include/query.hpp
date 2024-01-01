#pragma once

#include "typedefs.hpp"

#include <curl/curl.h>
#include <string>

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data);

class QueryHandler
{
private:
    void time_query();
    ::CURL *curl = NULL;
    bool run_timer = false;

public:
    QueryHandler();
    ~QueryHandler();
    ::str_response run_query(const ::str_request &request);
};
