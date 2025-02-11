#pragma once

#include <optional>
#include <string>

std::string select_chat_model();
std::string load_input_text(const std::optional<std::string> &input_file);
