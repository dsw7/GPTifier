#pragma once

#include <string>

typedef std::string req_str;

namespace requests
{
::req_str build_chat_request(const std::string &prompt, const std::string &model, const std::string &temp);
void print_request(const ::req_str &request);
} // namespace requests
