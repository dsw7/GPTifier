#include "parsers.hpp"

#include <fmt/core.h>
#include <stdexcept>

using json = nlohmann::json;

namespace {

void catch_deserialization_errors(json &results, const std::string &response)
{
    try {
        results = json::parse(response);
    } catch (const json::parse_error &e) {
        const std::string errmsg = fmt::format("Failed to parse completion. Error was:\n{}", e.what());
        throw std::runtime_error(errmsg);
    }
}

void catch_errors_from_openai(const json &results)
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

json parse_response(const std::string &response)
{
    json results;
    catch_deserialization_errors(results, response);
    catch_errors_from_openai(results);

    return results;
}

void print_raw_response(const std::string &response)
{
    const json results = parse_response(response);
    fmt::print("{}\n", results.dump(4));
}
