#pragma once

#include <json.hpp>
#include <string>

nlohmann::json response_to_json(const std::string &response);
