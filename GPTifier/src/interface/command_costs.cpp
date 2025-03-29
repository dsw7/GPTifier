#include "interface/command_costs.hpp"

#include "interface/help_messages.hpp"
#include "serialization/costs.hpp"
#include "utils.hpp"

#include <algorithm>
#include <ctime>
#include <fmt/core.h>
#include <getopt.h>
#include <string>

namespace {

struct Params {
    bool print_raw_json = false;
    std::string days = "30";
};

void read_cli(int argc, char **argv, Params &params)
{
    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "days", required_argument, 0, 'd' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hjd:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                cli::help_command_costs();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 'd':
                params.days = optarg;
                break;
            default:
                cli::exit_on_failure();
        }
    };
}

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
    Params params;
    read_cli(argc, argv, params);

    int days = utils::string_to_int(params.days);
    if (days < 1) {
        throw std::runtime_error("Days must be greater than 0");
    }

    std::time_t start_time = get_current_time_minus_days(days);

    int limit = 180;
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
