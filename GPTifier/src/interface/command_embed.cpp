#include "interface/command_embed.hpp"

#include "configs.hpp"
#include "datadir.hpp"
#include "interface/help_messages.hpp"
#include "serialization/embeddings.hpp"
#include "utils.hpp"
#include <getopt.h>

#include <fmt/core.h>
#include <iostream>
#include <json.hpp>
#include <optional>
#include <stdexcept>
#include <string>

namespace {

struct Params {
    std::optional<std::string> input = std::nullopt;
    std::optional<std::string> input_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> output_file = std::nullopt;
};

void read_cli(int argc, char **argv, Params &params)
{
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

    if (output_file) {
        filename = output_file.value();
    } else {
        filename = datadir::GPT_EMBEDDINGS.string();
    }

    nlohmann::json json;
    json["embedding"] = em.embedding;
    json["input"] = em.input;
    json["model"] = em.model;

    fmt::print("Dumping JSON to '{}'\n", filename);
    utils::write_to_file(filename, json.dump(2));
}

} // namespace

void command_embed(int argc, char **argv)
{
    Params params;
    read_cli(argc, argv, params);

    std::string text_to_embed;

    if (params.input) {
        text_to_embed = params.input.value();
    } else if (params.input_file) {
        fmt::print("Reading text from file: '{}'\n", params.input_file.value());
        text_to_embed = utils::read_from_file(params.input_file.value());
    } else {
        text_to_embed = read_text_from_stdin();
    }

    if (text_to_embed.empty()) {
        throw std::runtime_error("No input text provided anywhere");
    }

    std::string model;

    if (params.model) {
        model = params.model.value();
    } else if (configs.embeddings.model) {
        model = configs.embeddings.model.value();
    } else {
        throw std::runtime_error("No model provided via configuration file or command line");
    }

    const Embedding em = create_embedding(model, text_to_embed);
    export_embedding(em, params.output_file);

    print_sep();
}
