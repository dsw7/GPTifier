#include "ser_utils.hpp"

#include <fmt/core.h>
#include <stdexcept>

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

void throw_on_error_response(const std::string &response)
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

    throw std::runtime_error(errmsg);
}

void throw_on_ollama_error_response(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    if (not json.contains("error")) {
        throw std::runtime_error("An error occurred but 'error' key not found in the response JSON");
    }

    throw std::runtime_error(json["error"]);
}

} // namespace serialization
