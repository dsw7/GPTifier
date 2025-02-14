#include "command_embed.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "datadir.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "selectors.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <fstream>
#include <json.hpp>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

namespace {

void query_embeddings_api(const std::string &model, const std::string &input, json &results)
{
    const json data = { { "model", model }, { "input", input } };

    Curl curl;
    const std::string response = curl.create_embedding(data.dump());

    results = parse_response(response);
}

void export_embedding(const json &results)
{
    fmt::print("Dumping JSON to {}\n", datadir::GPT_EMBEDDINGS.string());
    std::ofstream st_filename(datadir::GPT_EMBEDDINGS);

    if (not st_filename.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'", datadir::GPT_EMBEDDINGS.string());
        throw std::runtime_error(errmsg);
    }

    st_filename << std::setw(2) << results;
    st_filename.close();
}

} // namespace

void command_embed(int argc, char **argv)
{
    ParamsEmbedding params = cli::get_opts_embed(argc, argv);

    if (not params.input.has_value()) {
        print_sep();
        params.input = select_input_text(params.input_file);
    }

    std::string model;

    if (params.model.has_value()) {
        model = params.model.value();
    } else {
        if (configs.embeddings.model.has_value()) {
            model = configs.embeddings.model.value();
        } else {
            throw std::runtime_error("No model provided via configuration file or command line");
        }
    }

    json results;
    query_embeddings_api(model, params.input.value(), results);

    results["input"] = params.input.value();

    print_sep();
    export_embedding(results);
    print_sep();
}
