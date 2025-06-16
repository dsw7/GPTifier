#pragma once

#include <json.hpp>
#include <string>

namespace serialization {
nlohmann::json response_to_json(const std::string &response);
}
