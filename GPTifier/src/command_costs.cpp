#include "command_costs.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "params.hpp"
#include "parsers.hpp"

#include <ctime>
#include <fmt/core.h>
#include <json.hpp>
#include <string>

using json = nlohmann::json;

namespace {

std::time_t get_current_time_minus_days(int days)
{
    if (days < 0) {
        return std::time(nullptr);
    }

    static std::time_t seconds_per_day = 86400;
    std::time_t offset = days * seconds_per_day;

    std::time_t now = std::time(nullptr);
    return now - offset;
}

} // namespace

void command_costs(int argc, char **argv)
{
    ParamsCosts params = cli::get_opts_get_costs(argc, argv);
    params.sanitize();

    std::time_t start_time = get_current_time_minus_days(std::get<int>(params.days));

    Curl curl;
    const std::string response = curl.get_costs(start_time);

    if (params.print_raw_json) {
        print_raw_response(response);
        return;
    }

    const json results = parse_response(response);
    fmt::print("{}\n", results.dump(4));
}
