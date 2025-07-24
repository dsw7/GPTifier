#include "command_embed.hpp"

#include "configs.hpp"
#include "datadir.hpp"
#include "embeddings.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
#include <optional>
#include <stdexcept>
#include <string>

namespace {

void help_embed()
{
    const std::string messages = R"(Get embedding representing a block of text. By default, the subcommand
will read text interactively. Long blocks of text can be read into the
program via file (see --read-from-file).

Usage:
  gpt embed [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid embedding model
  -i, --input=TEXT               Input text to embed
  -r, --read-from-file=FILENAME  Read input text to embed from a file
  -o, --output-file=FILENAME     Export embedding to FILENAME
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    std::optional<std::string> input = std::nullopt;
    std::optional<std::string> input_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> output_file = std::nullopt;
};

Parameters read_cli(int argc, char **argv)
{
    Parameters params;

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
                help_embed();
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
                utils::exit_on_failure();
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

void export_embedding(const serialization::Embedding &em, const std::optional<std::string> &output_file)
{
    std::string filename;

    if (output_file) {
        filename = output_file.value();
    } else {
        filename = datadir::GPT_EMBEDDINGS.string();
    }

    const nlohmann::json json = {
        { "embedding", em.embedding },
        { "input", em.input },
        { "model", em.model },
    };

    fmt::print("Dumping JSON to '{}'\n", filename);
    utils::write_to_file(filename, json.dump(2));
}

} // namespace

namespace commands {

void command_embed(int argc, char **argv)
{
    const Parameters params = read_cli(argc, argv);
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
    } else if (configs.model_embed) {
        model = configs.model_embed.value();
    } else {
        throw std::runtime_error("No model provided via configuration file or command line");
    }

    if (model.empty()) {
        throw std::runtime_error("Model is empty");
    }

    const serialization::Embedding em = serialization::create_embedding(model, text_to_embed);
    export_embedding(em, params.output_file);
}

} // namespace commands
