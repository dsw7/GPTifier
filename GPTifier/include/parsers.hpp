#pragma once

#include <json.hpp>
#include <optional>
#include <string>

std::optional<nlohmann::json> parse_response(const std::string &response);
