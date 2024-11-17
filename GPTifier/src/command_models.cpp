#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "json.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <iostream>
#include <string>

namespace {

void print_row(const std::string &id, const std::string &owned_by, const std::string &creation_time)
{
    std::cout << fmt::format("{:<40}{:<30}{}\n", id, owned_by, creation_time);
}

void print_models_response(const std::string &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error")) {
        std::string error = results["error"]["message"];
        reporting::print_error(error);
        return;
    }

    reporting::print_sep();
    print_row("Model ID", "Owner", "Creation time");
    reporting::print_sep();

    for (const auto &entry: results["data"]) {
        std::string id = entry["id"];
        std::string owned_by = entry["owned_by"];
        std::string creation_time = datetime_from_unix_timestamp(entry["created"]);
        print_row(id, owned_by, creation_time);
    }

    reporting::print_sep();
}

} // namespace

void command_models(const int argc, char **argv)
{
    cli::get_opts_models(argc, argv);

    std::string response = query_models_api();
    print_models_response(response);
}
