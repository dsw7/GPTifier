#include "serialization/parse_response.hpp"

#include <fmt/core.h>
#include <stdexcept>

namespace {

void catch_deserialization_errors(nlohmann::json &results, const std::string &response)
{
    try {
        results = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        const std::string errmsg = fmt::format("Failed to parse completion. Error was:\n{}", e.what());
        throw std::runtime_error(errmsg);
    }
}

void catch_errors_from_openai(const nlohmann::json &results)
{
    if (not results.contains("error")) {
        return;
    }

    if (results["error"].empty()) {
        return;
    }

    const std::string errmsg = results["error"]["message"];
    throw std::runtime_error(errmsg);
}

} // namespace

nlohmann::json parse_response(const std::string &response)
{
    nlohmann::json results;
    catch_deserialization_errors(results, response);
    catch_errors_from_openai(results);

    return results;
}
