#include "command_costs.hpp"

#include "api_openai_admin.hpp"
#include "cli.hpp"
#include "models.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "utils.hpp"
#include "validation.hpp"

#include <algorithm>
#include <ctime>
#include <fmt/core.h>
#include <json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

std::time_t get_current_time_minus_days(int days)
{
    if (days < 0) {
        return std::time(nullptr);
    }

    days--;

    static std::time_t seconds_per_day = 86400;
    std::time_t offset = days * seconds_per_day;

    std::time_t now = std::time(nullptr);
    return now - offset;
}

float unpack_results(const json &results, std::vector<models::CostsBucket> &buckets)
{
    float costs = 0.00;

    for (const auto &entry: results["data"]) {
        validation::is_bucket(entry);

        if (validation::is_bucket_empty(entry)) {
            continue;
        }

        const json cost_obj = entry["results"][0];
        validation::is_cost(cost_obj);

        float cost = cost_obj["amount"]["value"];
        costs += cost;

        models::CostsBucket bucket;
        bucket.end_time = entry["end_time"];
        bucket.start_time = entry["start_time"];
        bucket.cost = cost;
        bucket.org_id = cost_obj["organization_id"];

        buckets.push_back(bucket);
    }

    return costs;
}

void print_results(const std::vector<models::CostsBucket> &buckets, int days, float costs)
{
    print_sep();
    fmt::print("Overall usage (in USD) over {} days: {}\n", days, costs);
    print_sep();
    fmt::print("{:<25}{:<25}{:<25}{}\n", "Start time", "End time", "Usage (USD)", "Organization ID");
    print_sep();

    for (const auto &it: buckets) {
        const std::string dt_start = datetime_from_unix_timestamp(it.start_time);
        const std::string dt_end = datetime_from_unix_timestamp(it.end_time);
        fmt::print("{:<25}{:<25}{:<25}{}\n", dt_start, dt_end, it.cost, it.org_id);
    }

    print_sep();
}

} // namespace

void command_costs(int argc, char **argv)
{
    ParamsCosts params = cli::get_opts_get_costs(argc, argv);

    int limit = 180;
    int days = std::get<int>(params.days);
    std::time_t start_time = get_current_time_minus_days(days);

    OpenAIAdmin api;
    const std::string response = api.get_costs(start_time, limit);
    const json results = parse_response(response);

    if (params.print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    validation::is_page(results);

    std::vector<models::CostsBucket> buckets;
    float costs = unpack_results(results, buckets);

    std::sort(buckets.begin(), buckets.end(), [](const models::CostsBucket &left, const models::CostsBucket &right) {
        return left.start_time < right.start_time;
    });

    print_results(buckets, days, costs);
}
