#pragma once

#include <string>

namespace cli
{

struct ParamsEmbedding
{
    bool print_help = false;
    std::string input;
    std::string input_file;
    std::string model;
};

bool get_opts_models(const int argc, char **argv);
ParamsEmbedding get_opts_embed(const int argc, char **argv);
} // namespace cli
