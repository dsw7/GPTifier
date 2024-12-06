#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "json.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <map>
#include <string>

namespace {

struct Model {
    std::string id;
    std::string owned_by;
};

void print_row(int creation_time, const Model &model)
{
    const std::string datetime = datetime_from_unix_timestamp(creation_time);
    fmt::print("{:<40}{:<30}{}\n", model.id, model.owned_by, datetime);
}

void print_models_response(const std::string &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error")) {
        const std::string error = results["error"]["message"];
        reporting::print_error(error);
        return;
    }

    reporting::print_sep();
    fmt::print("{:<40}{:<30}{}\n", "Model ID", "Owner", "Creation time");

    reporting::print_sep();
    std::map<int, Model> models = {};

    for (const auto &entry: results["data"]) {
        Model model;
        model.id = entry["id"];
        model.owned_by = entry["owned_by"];
        models[entry["created"]] = model;
    }

    for (auto it = models.begin(); it != models.end(); ++it) {
        print_row(it->first, it->second);
    }

    reporting::print_sep();
}

} // namespace

void command_models(int argc, char **argv)
{
    cli::get_opts_models(argc, argv);

    const std::string response = query_models_api();
    print_models_response(response);
}
