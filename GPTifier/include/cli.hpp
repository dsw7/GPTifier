#pragma once

#include <string>

namespace cli
{

struct ParamsEmbedding
{
    std::string input;
    std::string input_file;
    std::string model;
};

struct ParamsRun
{
    bool enable_export = true;
    std::string json_dump_file;
    std::string model;
    std::string prompt;
    std::string prompt_file;
    std::string temperature = "1";
};

void get_opts_models(const int argc, char **argv);
ParamsEmbedding get_opts_embed(const int argc, char **argv);
ParamsRun get_opts_run(const int argc, char **argv);

} // namespace cli
