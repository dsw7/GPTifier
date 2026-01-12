#pragma once

#include <json.hpp>
#include <string>

namespace serialization {
nlohmann::json parse_json(const std::string &response);
void throw_on_openai_error_response(const std::string &response);
void throw_on_ollama_error_response(const std::string &response);
} // namespace serialization
