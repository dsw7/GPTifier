#include "command_costs.hpp"

#include "costs.hpp"
#include "utils.hpp"

#include <algorithm>
#include <ctime>
#include <fmt/core.h>
#include <getopt.h>
#include <string>

namespace {

void help_costs()
{
    const std::string messages = R"(Get OpenAI usage details. This is an administrative command and this command
will require that a valid OPENAI_ADMIN_KEY environment variable is set.

Usage:
  gpt costs [OPTIONS]

Options:
  -h, --help       Print help information and exit
  -j, --json       Print raw JSON response from OpenAI
  -d, --days=DAYS  Select number of days to go back
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    bool print_raw_json = false;
    std::string days = "30";
};

Parameters read_cli(int argc, char **argv)
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
        int c = getopt_long(argc, argv, "hjd:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_costs();
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

    return params;
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

void print_costs(const serialization::Costs &costs, int days)
{
    utils::separator();
    fmt::print("Overall usage (in USD) over {} days: {}\n", days, costs.total_cost);
    utils::separator();
    fmt::print("{:<25}{:<25}{:<25}{}\n", "Start time", "End time", "Usage (USD)", "Organization ID");
    utils::separator();

    for (const auto &bucket: costs.buckets) {
        const std::string dt_start = utils::datetime_from_unix_timestamp(bucket.start_time);
        const std::string dt_end = utils::datetime_from_unix_timestamp(bucket.end_time);
        fmt::print("{:<25}{:<25}{:<25}{}\n", dt_start, dt_end, bucket.cost, bucket.org_id);
    }

    utils::separator();
}

} // namespace

namespace commands {

void command_costs(int argc, char **argv)
{
    const Parameters params = read_cli(argc, argv);
    const int days = utils::string_to_int(params.days);

    if (days < 1) {
        throw std::runtime_error("Days must be greater than 0");
    }

    if (days > 60) {
        throw std::runtime_error("Days cannot be greater than 60");
    }

    const std::time_t start_time = get_current_time_minus_days(days);

    if (not params.print_raw_json) {
        fmt::print("Awaiting response from API...\n");
    }

    const int limit = 180;
    serialization::Costs costs = serialization::get_costs(start_time, limit);

    if (params.print_raw_json) {
        fmt::print("{}\n", costs.raw_response);
        return;
    }
    fmt::print("Complete!\n");

    std::sort(costs.buckets.begin(), costs.buckets.end());
    print_costs(costs, days);
}

} // namespace commands
