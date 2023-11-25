#pragma once

#include "typedefs.h"

#include <string>

namespace requests
{
::str_request build_chat_request(const std::string &prompt, const std::string &model, const std::string &temp);
void print_request(const ::str_request &request);
} // namespace requests
