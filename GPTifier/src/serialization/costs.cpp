#include "costs.hpp"

#include "api_openai_admin.hpp"
#include "response_to_json.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

void unpack_costs(const nlohmann::json &json, Costs &costs)
{
    for (const auto &entry: json["data"]) {
        if (entry["results"].empty()) {
            continue;
        }

        const nlohmann::json cost_obj = entry["results"][0];
        float cost = cost_obj["amount"]["value"];
        costs.total_cost += cost;

        CostsBucket bucket;
        bucket.end_time = entry["end_time"];
        bucket.start_time = entry["start_time"];
        bucket.cost = cost;
        bucket.org_id = cost_obj["organization_id"];

        costs.buckets.push_back(bucket);
    }
}

Costs unpack_response(const std::string &response)
{
    const nlohmann::json json = response_to_json(response);
    Costs costs;

    try {
        unpack_costs(json, costs);
    } catch (nlohmann::json::out_of_range &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    } catch (nlohmann::json::type_error &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return costs;
}

} // namespace

Costs get_costs(std::time_t start_time, int limit)
{
    networking::OpenAIAdmin api;
    const std::string response = api.get_costs(start_time, limit);

    Costs costs = unpack_response(response);
    costs.raw_response = response;
    return costs;
}
