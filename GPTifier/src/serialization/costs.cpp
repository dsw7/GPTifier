#include "costs.hpp"

#include "api_openai_admin.hpp"
#include "ser_utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

Costs unpack_costs(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    Costs costs;
    costs.raw_response = response;

    try {
        for (const auto &entry: json["data"]) {
            if (entry["results"].empty()) {
                continue;
            }

            const nlohmann::json cost_obj = entry["results"][0];
            float cost = 0.00f;

            if (cost_obj["amount"]["value"].is_number_float()) {
                cost = cost_obj["amount"]["value"];
            } else {
                cost = std::stof(cost_obj["amount"]["value"].get<std::string>());
            }

            costs.total_cost += cost;

            CostsBucket bucket;
            bucket.end_time = entry["end_time"];
            bucket.start_time = entry["start_time"];
            bucket.cost = cost;
            bucket.org_id = cost_obj["organization_id"];

            costs.buckets.push_back(bucket);
        }
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack models response: {}", e.what()));
    }

    return costs;
}

} // namespace

Costs get_costs(std::time_t start_time, int limit)
{
    const auto result = networking::get_costs(start_time, limit);

    if (not result) {
        throw_on_error_response(result.error().response);
    }

    return unpack_costs(result->response);
}

} // namespace serialization
