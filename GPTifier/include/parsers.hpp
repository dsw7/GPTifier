#pragma once

#include <json.hpp>
#include <string>

nlohmann::json parse_response(const std::string &response);
