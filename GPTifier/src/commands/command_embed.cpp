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

void help_embed_()
{
    const std::string messages = R"(Get embedding representing a block of text. By default, the subcommand
will read text interactively. Long blocks of text can be read into the
program via file (see --read-from-file).

Usage:
  gpt embed [OPTIONS]

Options:
  -h, --help                     Print help information and exit
  -m, --model=MODEL              Specify a valid embedding model
  -l, --use-local                Connect to locally hosted LLM as opposed to OpenAI
  -i, --input=TEXT               Input text to embed
  -r, --read-from-file=FILENAME  Read input text to embed from a file
  -o, --output-file=FILENAME     Export embedding to FILENAME
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    bool use_local = false;
    std::optional<std::string> input;
    std::optional<std::string> input_file;
    std::optional<std::string> model;
    std::optional<std::string> output_file;
};

Parameters read_cli_(const int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = { { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "use-local", no_argument, 0, 'l' },
            { "input", required_argument, 0, 'i' },
            { "output-file", required_argument, 0, 'o' },
            { "read-from-file", required_argument, 0, 'r' },
            { 0, 0, 0, 0 } };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hm:li:o:r:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_embed_();
                exit(EXIT_SUCCESS);
            case 'm':
                params.model = optarg;
                break;
            case 'l':
                params.use_local = true;
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

    if (params.output_file) {
        if (params.output_file.value().empty()) {
            throw std::runtime_error("Output file argument provided with no value");
        }
    }

    return params;
}

std::string read_input_text_from_stdin_()
{
    fmt::print(fg(white), "Input text to embed: ");
    std::string text;

    std::getline(std::cin, text);
    return text;
}

std::string get_text_to_embed_(const Parameters &params)
{
    std::string text_to_embed;

    if (params.input) {
        text_to_embed = params.input.value();
    } else if (params.input_file) {
        fmt::print("Reading text from file: '{}'\n", params.input_file.value());
        text_to_embed = utils::read_from_file(params.input_file.value());
    } else {
        text_to_embed = read_input_text_from_stdin_();
    }

    if (text_to_embed.empty()) {
        throw std::runtime_error("No input text provided anywhere");
    }

    return text_to_embed;
}

std::string select_model_(const Parameters &params)
{
    std::string model;

    if (params.model) {
        model = params.model.value();
    } else {
        if (params.use_local) {
            model = configs.model_embed_ollama.value();
        } else {
            model = configs.model_embed_openai.value();
        }
    }

    if (model.empty()) {
        throw std::runtime_error("Model is empty");
    }

    return model;
}

using serialization::Embedding;

void export_embedding_(const Embedding &embedding, const std::string &output_file)
{
    const nlohmann::json json = {
        { "embedding", embedding.embedding },
        { "input", embedding.input },
        { "model", embedding.model },
        { "source", embedding.source },
    };

    fmt::print("Dumping JSON to '{}'\n", output_file);
    utils::write_to_file(output_file, json.dump(2));
}

} // namespace

namespace commands {

void command_embed(const int argc, char **argv)
{
    const Parameters params = read_cli_(argc, argv);
    const std::string text_to_embed = get_text_to_embed_(params);
    const std::string model = select_model_(params);

    Embedding embedding;

    if (params.use_local) {
        embedding = serialization::create_ollama_embedding(model, text_to_embed);
    } else {
        embedding = serialization::create_openai_embedding(model, text_to_embed);
    }

    std::string output_file;

    if (params.output_file) {
        output_file = params.output_file.value();
    } else {
        output_file = datadir::GPT_EMBEDDINGS.string();
    }

    export_embedding_(embedding, output_file);
}

} // namespace commands
