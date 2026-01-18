#include "ser_utils.hpp"

#include <fmt/core.h>
#include <stdexcept>

namespace serialization {

std::string datetime_from_unix_timestamp(const std::time_t &timestamp)
{
    const std::tm *datetime = std::gmtime(&timestamp);
    char buffer[80];

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", datetime);
    return buffer;
}

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

void throw_on_openai_error_response(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    if (not json.contains("error")) {
        throw std::runtime_error("Malformed error response. No error object could be found");
    }

    if (json["error"].empty()) {
        throw std::runtime_error("An error occurred but error message is empty");
    }

    throw std::runtime_error(json["error"]["message"]);
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
