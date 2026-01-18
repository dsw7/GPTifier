#pragma once

#include <ctime>
#include <json.hpp>
#include <string>

namespace serialization {
std::string datetime_from_unix_timestamp(const std::time_t &timestamp);
nlohmann::json parse_json(const std::string &response);
void throw_on_openai_error_response(const std::string &response);
void throw_on_ollama_error_response(const std::string &response);
} // namespace serialization
