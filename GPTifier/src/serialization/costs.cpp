#include "costs.hpp"

#include "api_openai_admin.hpp"
#include "ser_utils.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

float get_costs_amount_(const nlohmann::json &cost_object)
{
    if (cost_object["amount"]["value"].is_number_float()) {
        return cost_object["amount"]["value"];
    };

    return std::stof(cost_object["amount"]["value"].get<std::string>());
}

CostsBucket unpack_individual_bucket_(const nlohmann::json &entry)
{
    CostsBucket bucket;
    bucket.start_time = entry["start_time"];
    bucket.start_time_dt_str = utils::datetime_from_unix_timestamp(entry["start_time"]);
    bucket.end_time = entry["end_time"];
    bucket.end_time_dt_str = utils::datetime_from_unix_timestamp(entry["end_time"]);
    bucket.cost = get_costs_amount_(entry["results"][0]);
    bucket.org_id = entry["results"][0]["organization_id"];
    return bucket;
}

Costs unpack_costs_response_(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    Costs costs;
    costs.raw_response = response;

    try {
        for (const auto &entry: json["data"]) {
            if (entry["results"].empty()) {
                continue;
            }

            const CostsBucket bucket = unpack_individual_bucket_(entry);
            costs.total_cost += bucket.cost;
            costs.buckets.push_back(bucket);
        }
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return costs;
}

} // namespace

Costs get_costs(const std::time_t start_time, const int limit)
{
    const auto result = networking::get_costs(start_time, limit);

    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    return unpack_costs_response_(result->response);
}

} // namespace serialization
