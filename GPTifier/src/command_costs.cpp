#include "command_costs.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "params.hpp"
#include "parsers.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <string>

using json = nlohmann::json;

void command_costs(int argc, char **argv)
{
    ParamsCosts params = cli::get_opts_get_costs(argc, argv);
    params.sanitize();

    Curl curl;
    const std::string response = curl.get_costs(1730419200);

    if (params.print_raw_json) {
        print_raw_response(response);
        return;
    }

    const json results = parse_response(response);
    fmt::print("{}\n", results.dump(4));
}
