#include "ser_utils.hpp"

#include <fmt/core.h>

namespace serialization {

nlohmann::json parse_json(const std::string &response)
{
    nlohmann::json json;

    try {
        json = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    return json;
}

Err unpack_error(const std::string &response, const long status_code)
{
    const nlohmann::json json = parse_json(response);

    std::string errmsg;

    if (json.contains("error")) {
        if (json["error"].empty()) {
            errmsg = "An error occurred but error message is empty";
        } else {
            errmsg = json["error"]["message"];
        }
    } else {
        errmsg = "Malformed error response. No error object could be found";
    }

    return Err { status_code, errmsg };
}

} // namespace serialization
