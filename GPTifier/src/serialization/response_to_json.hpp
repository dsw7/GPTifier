#pragma once

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

namespace serialization {
nlohmann::json response_to_json(const std::string &response);

template<typename T, typename UnpackFunction>
T unpack_response(const std::string &response, UnpackFunction unpack_func)
{
    const nlohmann::json json = response_to_json(response);
    T result;

    try {
        unpack_func(json, result);
    } catch (nlohmann::json::out_of_range &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    } catch (nlohmann::json::type_error &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return result;
}

} // namespace serialization
