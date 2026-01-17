#include "command_costs.hpp"

#include "costs.hpp"
#include "utils.hpp"

#include <algorithm>
#include <ctime>
#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <string>
#include <utility>

namespace {

void help_costs_()
{
    const std::string messages = R"(Get OpenAI usage details. This is an administrative command and this command
will require that a valid OPENAI_ADMIN_KEY environment variable is set.

Usage:
  gpt costs [OPTIONS]

Options:
  -h, --help       Print help information and exit
  -j, --json       Print raw JSON response from OpenAI
  -d, --days=DAYS  Select number of days to go back. Value
                   will be clamped between 1 and 60 days
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    bool print_raw_json = false;
    std::string days = "30";
};

Parameters read_cli_(const int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "days", required_argument, 0, 'd' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hjd:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_costs_();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 'd':
                params.days = optarg;
                break;
            default:
                utils::exit_on_failure();
        }
    };

    if (params.days.empty()) {
        throw std::runtime_error("Days argument is empty");
    }

    return params;
}

std::time_t get_current_time_minus_days_(int days)
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

using VecCostsBuckets = std::vector<serialization::CostsBucket>;

void print_individual_cost_buckets_(const VecCostsBuckets &buckets)
{
    fmt::print("{:<25}{:<25}{:<25}{}\n", "Start time", "End time", "Usage (USD)", "Organization ID");

    for (const auto &bucket: buckets) {
        const std::string dt_start = utils::datetime_from_unix_timestamp(bucket.start_time);
        const std::string dt_end = utils::datetime_from_unix_timestamp(bucket.end_time);
        fmt::print("{:<25}{:<25}{:<25}{}\n", dt_start, dt_end, bucket.cost, bucket.org_id);
    }
}

} // namespace

namespace commands {

void command_costs(const int argc, char **argv)
{
    const Parameters params = read_cli_(argc, argv);

    static int min_days = 1;
    static int max_days = 60;

    const int days = std::clamp(utils::string_to_int(params.days), min_days, max_days);
    const std::time_t start_time = get_current_time_minus_days_(days);

    if (not params.print_raw_json) {
        std::cout << "Getting usage statistics from OpenAI servers...\r" << std::flush;
    }

    const int limit = 180;
    const serialization::Costs costs = serialization::get_costs(start_time, limit);

    if (params.print_raw_json) {
        fmt::print("{}\n", costs.raw_response);
        return;
    }

    VecCostsBuckets buckets = std::move(costs.buckets);
    std::sort(buckets.begin(), buckets.end());

    print_individual_cost_buckets_(buckets);

    fmt::print("\nOverall usage (in USD) over {} days: {}\n", days, costs.total_cost);
}

} // namespace commands
