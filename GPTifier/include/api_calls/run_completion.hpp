#pragma once

#include <curl/curl.h>
#include <string>

std::string create_chat_completion(::CURL *curl, const std::string &request);
