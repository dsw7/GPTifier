#include "parsers.hpp"

#include "reporting.hpp"

#include <fmt/core.h>
#include <iostream>
#include <stdexcept>

namespace {

nlohmann::json catch_deserialization_errors(const std::string &response)
{
    // Throw an exception for low level errors
    nlohmann::json results;

    try {
        results = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        const std::string errmsg = fmt::format("Failed to parse completion. Error was:\n{}", e.what());
        throw std::runtime_error(errmsg);
    }

    return results;
}

} // namespace

std::optional<nlohmann::json> parse_response(const std::string &response)
{
    nlohmann::json results = catch_deserialization_errors(response);

    if (not results.contains("error")) {
        return results;
    }

    if (results["error"].empty()) {
        return results;
    }

    const std::string error = results["error"]["message"];
    reporting::print_error(error);

    return std::nullopt;
}
