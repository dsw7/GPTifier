#include "interface/command_costs.hpp"

#include "cli.hpp"
#include "params.hpp"
#include "serialization/costs.hpp"
#include "utils.hpp"

#include <algorithm>
#include <ctime>
#include <fmt/core.h>
#include <string>

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

void print_results(const Costs &costs, int days)
{
    print_sep();
    fmt::print("Overall usage (in USD) over {} days: {}\n", days, costs.total_cost);
    print_sep();
    fmt::print("{:<25}{:<25}{:<25}{}\n", "Start time", "End time", "Usage (USD)", "Organization ID");
    print_sep();

    for (const auto &it: costs.buckets) {
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

    Costs costs = get_costs(start_time, limit);

    if (params.print_raw_json) {
        fmt::print("{}\n", costs.raw_response);
        return;
    }

    std::sort(costs.buckets.begin(), costs.buckets.end(), [](const CostsBucket &left, const CostsBucket &right) {
        return left.start_time < right.start_time;
    });

    print_results(costs, days);
}
