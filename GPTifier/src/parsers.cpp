#include "parsers.hpp"

#include "reporting.hpp"

std::optional<nlohmann::json> parse_response(const std::string &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

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
