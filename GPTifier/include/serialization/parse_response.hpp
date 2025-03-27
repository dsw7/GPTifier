#pragma once

#include <json.hpp>
#include <string>

nlohmann::json parse_response(const std::string &response);
nlohmann::json response_to_json(const std::string &response);
