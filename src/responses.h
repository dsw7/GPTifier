#pragma once

#include "typedefs.h"

#include <string>

namespace responses
{
void print_chat_completion_response(const ::str_response &response);
void dump_chat_completion_response(const ::str_response &response, const std::string &filename);
} // namespace responses
