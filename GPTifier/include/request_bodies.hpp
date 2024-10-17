#pragma once

#include <string>

std::string get_chat_completion_request_body(const std::string &model, const std::string &prompt, float temperature);
std::string get_embedding_request_body(const std::string &model, const std::string &input);
