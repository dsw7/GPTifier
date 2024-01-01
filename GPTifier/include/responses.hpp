#pragma once

#include "typedefs.hpp"

#include <string>

namespace responses
{
void print_chat_completion_response(const ::str_response &response);
void write_message_to_file(const std::string &message);
void export_chat_completion_response(const ::str_response &response);
void dump_chat_completion_response(const ::str_response &response);
} // namespace responses
