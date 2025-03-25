#include "serialization/costs.hpp"

#include "networking/api_openai_admin.hpp"
#include "serialization/parse_response.hpp"
#include "validation.hpp"

#include <json.hpp>

namespace {

void unpack_costs(const nlohmann::json &results, Costs &costs)
{
    for (const auto &entry: results["data"]) {
        validation::is_bucket(entry);

        if (validation::is_bucket_empty(entry)) {
            continue;
        }

        const nlohmann::json cost_obj = entry["results"][0];
        validation::is_cost(cost_obj);

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
    OpenAIAdmin api;
    const std::string response = api.get_costs(start_time, limit);
    const nlohmann::json results = parse_response(response);

    validation::is_page(results);

    Costs costs;
    costs.raw_response = response;

    unpack_costs(results, costs);
    return costs;
}
