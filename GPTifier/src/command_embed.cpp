#include "command_embed.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "datadir.hpp"
#include "input_selection.hpp"
#include "reporting.hpp"

#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>

namespace
{

nlohmann::json select_embedding_model(const std::string &model)
{
    nlohmann::json body = {};

    // I.e. model was passed via command line option
    if (not model.empty())
    {
        body["model"] = model;
        return body;
    }

    // I.e. load default model from configuration file
    if (not configs.embeddings.model.empty())
    {
        body["model"] = configs.embeddings.model;
        return body;
    }

    throw std::runtime_error("No model provided via configuration file or command line");
}

std::string get_post_fields(const cli::ParamsEmbedding &params)
{
    nlohmann::json body = select_embedding_model(params.model);

    body["input"] = params.input;
    std::string post_fields = body.dump(2);

    reporting::print_sep();
    reporting::print_request(post_fields);
    reporting::print_sep();

    return post_fields;
}

void export_embedding(const std::string &response, const std::string &input)
{
    nlohmann::json results = nlohmann::json::parse(response);
    results["input"] = input;

    if (results.contains("error"))
    {
        std::string error = results["error"]["message"];
        reporting::print_error(error);
    }

    std::cout << fmt::format("Dumping JSON to {}\n", datadir::GPT_EMBEDDINGS);
    std::ofstream st_filename(datadir::GPT_EMBEDDINGS);

    if (not st_filename.is_open())
    {
        std::string errmsg = fmt::format("Unable to open '{}'", datadir::GPT_EMBEDDINGS);
        throw std::runtime_error(errmsg);
    }

    st_filename << std::setw(2) << results;
    st_filename.close();

    reporting::print_sep();
}

} // namespace

void command_embed(const int argc, char **argv)
{
    cli::ParamsEmbedding params = cli::get_opts_embed(argc, argv);

    if (params.input.empty())
    {
        reporting::print_sep();
        params.input = load_input_text(params.input_file);
    }

    std::string post_fields = get_post_fields(params);
    std::string response = query_embeddings_api(post_fields);
    export_embedding(response, params.input);
}
