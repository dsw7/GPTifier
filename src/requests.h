#pragma once

#include <string>

namespace requests
{
std::string build_chat_request(const std::string &prompt, const std::string &model, const std::string &temp);
}
