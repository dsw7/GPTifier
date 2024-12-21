#pragma once

#include <optional>
#include <string>

namespace cli {

void get_opts_models(int argc, char **argv);
std::optional<std::string> get_opts_short(int argc, char **argv);

struct ParamsEmbedding {
    std::optional<std::string> input = std::nullopt;
    std::optional<std::string> input_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
};

ParamsEmbedding get_opts_embed(int argc, char **argv);

struct ParamsRun {
    bool enable_export = true;
    std::optional<std::string> json_dump_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> prompt = std::nullopt;
    std::optional<std::string> prompt_file = std::nullopt;
    std::string temperature = "1";
};

ParamsRun get_opts_run(int argc, char **argv);

struct ParamsFineTune {
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> training_file = std::nullopt;
};

ParamsFineTune get_opts_create_fine_tuning_job(int argc, char **argv);

} // namespace cli
