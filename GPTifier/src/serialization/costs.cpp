#include "costs.hpp"

#include "api_openai_admin.hpp"
#include "response_to_json.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

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

} // namespace

Costs get_costs(std::time_t start_time, int limit)
{
    const std::string response = networking::get_costs(start_time, limit);
    Costs costs = unpack_response<Costs>(response, unpack_costs);
    costs.raw_response = response;
    return costs;
}

} // namespace serialization
