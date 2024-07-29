#include "command_embed.hpp"

#include "api.hpp"
#include "configs.hpp"
#include "help_messages.hpp"
#include "input_selection.hpp"
#include "utils.hpp"

#include <curl/curl.h>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>

struct EmbeddingParameters
{
    bool print_help = false;
    std::string input;
    std::string input_file;
    std::string model;
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

void select_embedding_model(nlohmann::json &body, const std::string &model)
{
    // I.e. model was passed via command line option
    if (not model.empty())
    {
        body["model"] = model;
        return;
    }

    // I.e. load default model from configuration file
    if (not ::configs.embeddings.model.empty())
    {
        body["model"] = ::configs.embeddings.model;
        return;
    }

    throw std::runtime_error("No model provided via configuration file or command line");
}

void get_post_fields(std::string &post_fields, const EmbeddingParameters &params)
{
    nlohmann::json body = {};
    ::select_embedding_model(body, params.model);

    body["input"] = params.input;
    post_fields = body.dump(2);

    ::print_separator();
    std::cout << "\033[1mRequest:\033[0m " + post_fields + '\n';
    ::print_separator();
}

void query_embeddings_api(::CURL *curl, const std::string &post_fields, std::string &response)
{
    static std::string url_embeddings = "https://api.openai.com/v1/embeddings";
    ::curl_easy_setopt(curl, ::CURLOPT_URL, url_embeddings.c_str());

    ::curl_easy_setopt(curl, ::CURLOPT_POST, 1L);
    ::curl_easy_setopt(curl, ::CURLOPT_POSTFIELDS, post_fields.c_str());

    ::curl_easy_setopt(curl, ::CURLOPT_WRITEDATA, &response);
    ::CURLcode rv = ::curl_easy_perform(curl);

    if (rv != ::CURLE_OK)
    {
        std::string errmsg = "Failed to run query. " + std::string(::curl_easy_strerror(rv));
        throw std::runtime_error(errmsg);
    }
}

void export_embedding(const std::string &response, const std::string &input)
{
    nlohmann::json results = nlohmann::json::parse(response);
    results["input"] = input;

    if (results.contains("error"))
    {
        std::string error = results["error"]["message"];
        std::cerr << "\033[1mError:\033[31m " + error + "\033[0m\n";
    }

    std::string path_embedding_json = ::get_proj_home_dir() + "/embeddings.gpt";

    std::cout << "Dumping JSON to " + path_embedding_json + '\n';
    std::ofstream st_filename(path_embedding_json);

    if (not st_filename.is_open())
    {
        throw std::runtime_error("Unable to open '" + path_embedding_json + "'");
    }

    static short int indent_pretty_print = 2;
    st_filename << std::setw(indent_pretty_print) << results;
    st_filename.close();

    ::print_separator();
}

void command_embed(const int argc, char **argv)
{
    EmbeddingParameters params;
    ::read_cli_embed(argc, argv, params);

    if (params.print_help)
    {
        help::command_embed();
        return;
    }

    if (params.input.empty())
    {
        ::print_separator();
        ::load_input_text(params.input, params.input_file);
    }

    std::string post_fields;
    ::get_post_fields(post_fields, params);

    Curl curl;
    std::string response;
    ::query_embeddings_api(curl.handle, post_fields, response);

    ::export_embedding(response, params.input);
}
