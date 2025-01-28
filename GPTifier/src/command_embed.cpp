#include "command_embed.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "datadir.hpp"
#include "input_selection.hpp"
#include "json.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <fstream>
#include <stdexcept>
#include <string>

namespace {

void export_embedding(const std::string &response, const std::string &input)
{
    nlohmann::json results = parse_response(response);
    results["input"] = input;

    fmt::print("Dumping JSON to {}\n", datadir::GPT_EMBEDDINGS.string());

    std::ofstream st_filename(datadir::GPT_EMBEDDINGS);

    if (not st_filename.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'", datadir::GPT_EMBEDDINGS.string());
        throw std::runtime_error(errmsg);
    }

    st_filename << std::setw(2) << results;
    st_filename.close();

    print_sep();
}

} // namespace

void command_embed(int argc, char **argv)
{
    ParamsEmbedding params = cli::get_opts_embed(argc, argv);

    if (not params.input.has_value()) {
        print_sep();
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

    const std::string response = api::create_embedding(model, params.input.value());

    print_sep();
    export_embedding(response, params.input.value());
}
