#pragma once

#include "typedefs.hpp"

namespace responses
{
void print_chat_completion_response(const ::str_response &response);
void export_chat_completion_response(const ::str_response &response);
void dump_chat_completion_response(const ::str_response &response);
} // namespace responses
