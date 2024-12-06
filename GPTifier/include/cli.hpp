#pragma once

#include <optional>
#include <string>

namespace cli {

struct ParamsEmbedding {
    std::optional<std::string> input = std::nullopt;
    std::optional<std::string> input_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
};

struct ParamsRun {
    bool enable_export = true;
    std::optional<std::string> json_dump_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> prompt = std::nullopt;
    std::optional<std::string> prompt_file = std::nullopt;
    std::string temperature = "1";
};

void get_opts_models(int argc, char **argv);
std::optional<std::string> get_opts_short(int argc, char **argv);
ParamsEmbedding get_opts_embed(int argc, char **argv);
ParamsRun get_opts_run(int argc, char **argv);

} // namespace cli
