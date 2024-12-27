#pragma once

#include <optional>
#include <string>

namespace cli {

struct ParamsRun {
    bool enable_export = true;
    std::optional<std::string> json_dump_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> prompt = std::nullopt;
    std::optional<std::string> prompt_file = std::nullopt;
    std::string temperature = "1";
};
ParamsRun get_opts_run(int argc, char **argv);

struct ParamsShort {
    bool print_raw_json = false;
    std::optional<std::string> prompt = std::nullopt;
};
ParamsShort get_opts_short(int argc, char **argv);

bool get_opts_models(int argc, char **argv);

struct ParamsEmbedding {
    std::optional<std::string> input = std::nullopt;
    std::optional<std::string> input_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
};
ParamsEmbedding get_opts_embed(int argc, char **argv);

bool get_opts_files_list(int argc, char **argv);

struct ParamsFineTune {
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> training_file = std::nullopt;
};
ParamsFineTune get_opts_create_fine_tuning_job(int argc, char **argv);

struct ParamsGetFineTuningJobs {
    bool print_raw_json = false;
    std::optional<std::string> limit = std::nullopt;
};
ParamsGetFineTuningJobs get_opts_get_fine_tuning_jobs(int argc, char **argv);

} // namespace cli
