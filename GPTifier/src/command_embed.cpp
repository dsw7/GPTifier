#include "command_embed.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "datadir.hpp"
#include "input_selection.hpp"
#include "json.hpp"
#include "reporting.hpp"
#include "request_bodies.hpp"

#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void export_embedding(const std::string &response, const std::string &input)
{
    nlohmann::json results = nlohmann::json::parse(response);
    results["input"] = input;

    if (results.contains("error")) {
        std::string error = results["error"]["message"];
        reporting::print_error(error);
    }

    std::cout << fmt::format("Dumping JSON to {}\n", datadir::GPT_EMBEDDINGS);
    std::ofstream st_filename(datadir::GPT_EMBEDDINGS);

    if (not st_filename.is_open()) {
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

    if (not params.input.has_value()) {
        reporting::print_sep();
        params.input = load_input_text(params.input_file);
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

    std::string request_body = get_embedding_request_body(model, params.input.value());

    reporting::print_sep();
    reporting::print_request(request_body);
    reporting::print_sep();

    std::string response = query_embeddings_api(request_body);
    export_embedding(response, params.input.value());
}
