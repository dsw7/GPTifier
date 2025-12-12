#pragma once

#include <json.hpp>

#include <string>

namespace serialization {

nlohmann::json parse_json(const std::string &response);
void throw_on_error(const std::string &response);

struct Err {
    long status_code = -1;
    std::string errmsg;
};

Err unpack_error(const std::string &response, const long status_code);

} // namespace serialization
