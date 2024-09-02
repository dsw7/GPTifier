#include "command_embed.hpp"

#include "api.hpp"
#include "configs.hpp"
#include "help_messages.hpp"
#include "input_selection.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>

namespace
{

struct ParamsEmbedding
{
    bool print_help = false;
    std::string input;
    std::string input_file;
    std::string model;
};

ParamsEmbedding read_cli_embed(const int argc, char **argv)
{
    ParamsEmbedding params;

    while (true)
    {
        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"model", required_argument, 0, 'm'},
                                               {"input", required_argument, 0, 'i'},
                                               {"read-from-file", required_argument, 0, 'r'},
                                               {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "hm:i:r:", long_options, &option_index);

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
            params.model = optarg;
            break;
        case 'i':
            params.input = optarg;
            break;
        case 'r':
            params.input_file = optarg;
            break;
        default:
            std::cerr << "Try running with -h or --help for more information\n";
            exit(EXIT_FAILURE);
        }
    }

    return params;
}

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

std::string get_post_fields(const ParamsEmbedding &params)
{
    nlohmann::json body = select_embedding_model(params.model);

    body["input"] = params.input;
    std::string post_fields = body.dump(2);

    Reporting::print_sep();
    Reporting::print_request(post_fields);
    Reporting::print_sep();

    return post_fields;
}

void export_embedding(const std::string &response, const std::string &input)
{
    nlohmann::json results = nlohmann::json::parse(response);
    results["input"] = input;

    if (results.contains("error"))
    {
        std::string error = results["error"]["message"];
        Reporting::print_error(error);
    }

    std::string path_embedding_json = get_proj_home_dir() + "/embeddings.gpt";

    std::cout << "Dumping JSON to " + path_embedding_json + '\n';
    std::ofstream st_filename(path_embedding_json);

    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open '" + path_embedding_json + "'");
    }

    st_filename << std::setw(2) << results;
    st_filename.close();

    Reporting::print_sep();
}

} // namespace

void command_embed(const int argc, char **argv)
{
    ParamsEmbedding params = read_cli_embed(argc, argv);

    if (params.print_help)
    {
        help::command_embed();
        return;
    }

    if (params.input.empty())
    {
        Reporting::print_sep();
        load_input_text(params.input, params.input_file);
    }

    std::string post_fields = get_post_fields(params);
    std::string response = query_embeddings_api(post_fields);
    export_embedding(response, params.input);
}
