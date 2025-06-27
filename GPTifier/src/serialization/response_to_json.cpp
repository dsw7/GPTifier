#include "response_to_json.hpp"

#include <stdexcept>

namespace serialization {

nlohmann::json response_to_json(const std::string &response)
{
    nlohmann::json json;

    try {
        json = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    if (json.contains("error")) {
        if (not json["error"].empty()) {
            throw std::runtime_error(json["error"]["message"]);
        }
    }

    return json;
}

} // namespace serialization
