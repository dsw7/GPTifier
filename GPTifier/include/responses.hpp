#pragma once

#include <string>

namespace responses
{
void print_chat_completion_response(const std::string &response);
void export_chat_completion_response(const std::string &response);
void dump_chat_completion_response(const std::string &response);
} // namespace responses
