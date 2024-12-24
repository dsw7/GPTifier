#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "json.hpp"
#include "parsers.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <string>
#include <tuple>
#include <vector>

namespace {

typedef std::tuple<int, std::string, std::string> tup_model;
typedef std::vector<tup_model> vec_models;

bool is_fine_tuning_model(const std::string &model)
{
    return model.compare(0, 3, "ft:") == 0;
}

void print_models(vec_models &models)
{
    reporting::print_sep();
    fmt::print("{:<25}{:<35}{}\n", "Creation time", "Owner", "Model ID");
    reporting::print_sep();

    std::sort(models.begin(), models.end(), [](const tup_model &a, const tup_model &b) {
        if (std::get<0>(a) != std::get<0>(b)) {
            return std::get<0>(a) < std::get<0>(b);
        }

        // otherwise sort by model ID
        return std::get<2>(a) < std::get<2>(b);
    });

    for (auto it = models.begin(); it != models.end(); ++it) {
        const std::string datetime = datetime_from_unix_timestamp(std::get<0>(*it));
        fmt::print("{:<25}{:<35}{}\n", datetime, std::get<1>(*it), std::get<2>(*it));
    }

    reporting::print_sep();
}

void print_models_response(const std::string &response)
{
    const nlohmann::json results = parse_response(response);

    vec_models openai_models = {};
    vec_models user_models = {};

    for (const auto &entry: results["data"]) {
        if (is_fine_tuning_model(entry["id"])) {
            user_models.push_back({ entry["created"], entry["owned_by"], entry["id"] });
        } else {
            openai_models.push_back({ entry["created"], entry["owned_by"], entry["id"] });
        }
    }

    fmt::print("> OpenAI models:\n");
    print_models(openai_models);

    if (not user_models.empty()) {
        fmt::print("\n> User models:\n");
        print_models(user_models);
    }
}

} // namespace

void command_models(int argc, char **argv)
{
    cli::get_opts_models(argc, argv);

    const std::string response = query_models_api();
    print_models_response(response);
}
