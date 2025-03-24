#include "command_embed.hpp"

#include "cli.hpp"
#include "configs.hpp"
#include "datadir.hpp"
#include "networking/api_openai_user.hpp"
#include "params.hpp"
#include "serialization/embeddings.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <optional>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

namespace {

std::string read_text_from_stdin()
{
    fmt::print(fg(white), "Input text to embed: ");
    std::string text;

    std::getline(std::cin, text);
    return text;
}

void export_embedding(const Embedding &em, const std::optional<std::string> &output_file)
{
    std::string filename;

    if (output_file.has_value()) {
        filename = output_file.value();
    } else {
        filename = datadir::GPT_EMBEDDINGS.string();
    }

    fmt::print("Dumping JSON to '{}'\n", filename);
    std::ofstream st_filename(filename);

    if (not st_filename.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'", filename);
        throw std::runtime_error(errmsg);
    }

    json results;

    results["embedding"] = em.embedding;
    results["input"] = em.input;
    results["model"] = em.model;

    st_filename << std::setw(2) << results;
    st_filename.close();
}

} // namespace

void command_embed(int argc, char **argv)
{
    ParamsEmbedding params = cli::get_opts_embed(argc, argv);

    std::string text_to_embed;

    if (params.input.has_value()) {
        text_to_embed = params.input.value();
    } else if (params.input_file.has_value()) {
        text_to_embed = read_text_from_file(params.input_file.value());
    } else {
        text_to_embed = read_text_from_stdin();
    }

    if (text_to_embed.empty()) {
        throw std::runtime_error("No input text provided anywhere");
    }

    std::string model;

    if (params.model.has_value()) {
        model = params.model.value();
    } else if (configs.embeddings.model.has_value()) {
        model = configs.embeddings.model.value();
    } else {
        throw std::runtime_error("No model provided via configuration file or command line");
    }

    const Embedding em = create_embedding(model, text_to_embed);
    export_embedding(em, params.output_file);

    print_sep();
}
