#include "parsers.hpp"

#include <fmt/core.h>
#include <stdexcept>

namespace {

void catch_deserialization_errors(nlohmann::json &json, const std::string &response)
{
    try {
        json = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        const std::string errmsg = fmt::format("Failed to parse completion. Error was:\n{}", e.what());
        throw std::runtime_error(errmsg);
    }
}

void catch_errors_from_openai(const nlohmann::json &json)
{
    if (not json.contains("error")) {
        return;
    }

    if (json["error"].empty()) {
        return;
    }

    const std::string errmsg = json["error"]["message"];
    throw std::runtime_error(errmsg);
}

} // namespace

nlohmann::json parse_response(const std::string &response)
{
    nlohmann::json json;

    catch_deserialization_errors(json, response);
    catch_errors_from_openai(json);

    return json;
}
