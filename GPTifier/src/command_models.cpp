#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "models.hpp"
#include "parsers.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

bool is_fine_tuning_model(const std::string &model)
{
    return model.compare(0, 3, "ft:") == 0;
}

void print_models(std::vector<models::Model> &models)
{
    print_sep();
    fmt::print("{:<25}{:<35}{}\n", "Creation time", "Owner", "Model ID");
    print_sep();

    models::sort(models);

    for (auto it = models.begin(); it != models.end(); ++it) {
        it->print();
    }

    print_sep();
}

void print_models_response(const json &response)
{
    std::vector<models::Model> openai_models;
    std::vector<models::Model> user_models;

    for (const auto &entry: response["data"]) {
        if (is_fine_tuning_model(entry["id"])) {
            user_models.push_back({ entry["created"], entry["id"], entry["owned_by"] });
        } else {
            openai_models.push_back({ entry["created"], entry["id"], entry["owned_by"] });
        }
    }

    fmt::print("> OpenAI models:\n");
    fmt::print("> Number of models: {}\n", openai_models.size());
    print_models(openai_models);

    if (not user_models.empty()) {
        fmt::print("\n> User models:\n");
        fmt::print("> Number of models: {}\n", user_models.size());
        print_models(user_models);
    }
}

} // namespace

void command_models(int argc, char **argv)
{
    bool print_raw = cli::get_opts_models(argc, argv);

    Curl curl;
    const std::string response = curl.get_models();

    if (print_raw) {
        print_raw_response(response);
        return;
    }

    const json results = parse_response(response);
    print_models_response(results);
}
