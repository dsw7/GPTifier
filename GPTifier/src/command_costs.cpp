#include "command_costs.hpp"

#include "api_openai_admin.hpp"
#include "cli.hpp"
#include "models.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "utils.hpp"
#include "validation.hpp"

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

void print_results(const json &results, int days)
{
    std::vector<models::CostsBucket> buckets;
    float costs = 0.00;

    for (const auto &entry: results["data"]) {
        if (entry["results"].empty()) {
            continue;
        }

        float cost = entry["results"][0]["amount"]["value"];
        costs += cost;

        models::CostsBucket bucket;
        bucket.end_time = entry["end_time"];
        bucket.start_time = entry["start_time"];
        bucket.cost = cost;
        bucket.org_id = entry["results"][0]["organization_id"];

        buckets.push_back(bucket);
    }

    print_sep();
    fmt::print("Overall usage (in USD) over {} days: {}\n", days, costs);
    print_sep();

    fmt::print("{:<25}{:<25}{:<25}{}\n", "Start time", "End time", "Usage (USD)", "Organization ID");
    print_sep();

    models::sort(buckets);

    for (auto it = buckets.begin(); it != buckets.end(); it++) {
        it->print();
    }

    print_sep();
}

} // namespace

void command_costs(int argc, char **argv)
{
    ParamsCosts params = cli::get_opts_get_costs(argc, argv);
    params.sanitize();

    std::time_t start_time = get_current_time_minus_days(std::get<int>(params.days));
    int limit = 180;

    OpenAIAdmin api;
    const std::string response = api.get_costs(start_time, limit);
    const json results = parse_response(response);

    if (params.print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    if (not validation::is_costs_list(results)) {
        throw std::runtime_error("Response from OpenAI is not a list of costs");
    }

    print_results(results, std::get<int>(params.days));
}
