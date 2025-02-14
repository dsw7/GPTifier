#include "command_embed.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "datadir.hpp"
#include "models.hpp"
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

models::Embedding query_embeddings_api(const std::string &model, const std::string &input)
{
    const json data = { { "model", model }, { "input", input } };

    Curl curl;
    const std::string response = curl.create_embedding(data.dump());
    const json results = parse_response(response);

    models::Embedding embedding;

    try {
        embedding.embedding = results["data"][0]["embedding"].template get<std::vector<float>>();
        embedding.input = input;
        embedding.model = results["model"];
    } catch (const json::exception &e) {
        const std::string errmsg = fmt::format("Malformed response from OpenAI. Error was:\n{}", e.what());
        throw std::runtime_error(errmsg);
    }

    return embedding;
}

void export_embedding(const models::Embedding &embedding)
{
    fmt::print("Dumping JSON to {}\n", datadir::GPT_EMBEDDINGS.string());
    std::ofstream st_filename(datadir::GPT_EMBEDDINGS);

    if (not st_filename.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'", datadir::GPT_EMBEDDINGS.string());
        throw std::runtime_error(errmsg);
    }

    const json results = embedding.jsonify();
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

    const models::Embedding embedding = query_embeddings_api(model, params.input.value());

    print_sep();
    export_embedding(embedding);
    print_sep();
}
