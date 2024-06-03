#include "command_embed.hpp"

#include "help_messages.hpp"
#include "utils.hpp"

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

struct EmbeddingParameters
{
    bool print_help = false;
    std::string input;
    std::string input_file;
    std::string model = "text-embedding-ada-002";
};

void read_cli_embed(const int argc, char **argv, EmbeddingParameters &params)
{
    while (true)
    {
        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"model", required_argument, 0, 'm'},
                                               {"input", required_argument, 0, 'i'},
                                               {"read-from-file", required_argument, 0, 'r'},
                                               {0, 0, 0, 0}};

        int option_index = 0;
        int c = ::getopt_long(argc, argv, "hm:i:r:", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            params.print_help = true;
            break;
        case 'm':
            params.model = ::optarg;
            break;
        case 'i':
            params.input = ::optarg;
            break;
        case 'r':
            params.input_file = ::optarg;
            break;
        default:
            std::cerr << "Try running with -h or --help for more information\n";
            ::exit(EXIT_FAILURE);
        }
    }
}

void read_input_text_from_file(const std::string &input_file, std::string &input)
{
    std::cout << "Reading input text from file: " + input_file + '\n';
    std::ifstream file(input_file);

    if (not file.is_open())
    {
        throw std::runtime_error("Could not open file '" + input_file + "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    input = buffer.str();

    file.close();
}

void get_input(EmbeddingParameters &params)
{
    // Input text was passed via command line
    if (not params.input.empty())
    {
        return;
    }

    ::print_separator();

    // Input text was passed via file
    if (not params.input_file.empty())
    {
        ::read_input_text_from_file(params.input_file, params.input);
        return;
    }

    // Otherwise default to reading the input text from stdin
    std::cout << "\033[1mInput:\033[0m ";
    std::getline(std::cin, params.input);

    // If still empty then we cannot proceed
    if (params.input.empty())
    {
        throw std::runtime_error("No input text provided anywhere. Cannot proceed");
    }
}

void get_post_fields(std::string &post_fields, const EmbeddingParameters &params)
{
    nlohmann::json body = {};

    body["input"] = params.input;
    body["model"] = params.model;

    post_fields = body.dump(2);

    ::print_separator();
    std::cout << "\033[1mRequest:\033[0m " + post_fields + '\n';
    ::print_separator();
}

void command_embed(const int argc, char **argv)
{
    EmbeddingParameters embed_parameters;
    ::read_cli_embed(argc, argv, embed_parameters);

    if (embed_parameters.print_help)
    {
        help::command_embed();
        return;
    }

    ::get_input(embed_parameters);

    std::string post_fields;
    ::get_post_fields(post_fields, embed_parameters);
}
