#include "command_costs.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "utils.hpp"

#include <ctime>
#include <fmt/core.h>
#include <json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

struct CostsBucket {
    std::time_t end_time;
    std::time_t start_time;
    std::string org_id;

    void print()
    {
        const std::string start_time = datetime_from_unix_timestamp(this->start_time);
        const std::string end_time = datetime_from_unix_timestamp(this->end_time);

        fmt::print("{:<25}{:<25}{}\n", start_time, end_time, this->org_id);
    }
};

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

void print_results(const json &results)
{
    std::vector<CostsBucket> buckets;

    for (const auto &entry: results["data"]) {
        if (entry["results"].empty()) {
            continue;
        }

        buckets.push_back({ entry["end_time"], entry["start_time"], entry["results"][0]["organization_id"] });
    }

    print_sep();
    fmt::print("{:<25}{:<25}{}\n", "Start time", "End time", "Model ID");
    print_sep();

    for (auto it = buckets.begin(); it != buckets.end(); it++) {
        it->print();
    }
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
    print_results(results);
}
