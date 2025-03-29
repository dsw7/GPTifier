#include "interface/command_embed.hpp"

#include "configs.hpp"
#include "datadir.hpp"
#include "help_messages.hpp"
#include "interface/params.hpp"
#include "serialization/embeddings.hpp"
#include "utils.hpp"
#include <getopt.h>

#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <optional>
#include <stdexcept>
#include <string>

namespace {

ParamsEmbedding read_cli(int argc, char **argv)
{
    ParamsEmbedding params;

    while (true) {
        static struct option long_options[] = { { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "input", required_argument, 0, 'i' },
            { "output-file", required_argument, 0, 'o' },
            { "read-from-file", required_argument, 0, 'r' },
            { 0, 0, 0, 0 } };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hm:i:o:r:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                cli::help_command_embed();
                exit(EXIT_SUCCESS);
            case 'm':
                params.model = optarg;
                break;
            case 'i':
                params.input = optarg;
                break;
            case 'o':
                params.output_file = optarg;
                break;
            case 'r':
                params.input_file = optarg;
                break;
            default:
                cli::exit_on_failure();
        }
    }

    return params;
}

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

    nlohmann::json results;

    results["embedding"] = em.embedding;
    results["input"] = em.input;
    results["model"] = em.model;

    st_filename << std::setw(2) << results;
    st_filename.close();
}

} // namespace

void command_embed(int argc, char **argv)
{
    ParamsEmbedding params = read_cli(argc, argv);

    std::string text_to_embed;

    if (params.input.has_value()) {
        text_to_embed = params.input.value();
    } else if (params.input_file.has_value()) {
        fmt::print("Reading text from file: '{}'\n", params.input_file.value());
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
