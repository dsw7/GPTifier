#pragma once

#include <optional>
#include <string>

struct ParamsRun {
    bool enable_export = true;
    std::optional<std::string> json_dump_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> prompt = std::nullopt;
    std::optional<std::string> prompt_file = std::nullopt;
    std::string temperature = "1";
};

struct ParamsShort {
    bool print_raw_json = false;
    std::optional<std::string> prompt = std::nullopt;
};

struct ParamsEmbedding {
    std::optional<std::string> input = std::nullopt;
    std::optional<std::string> input_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
};

struct ParamsFineTune {
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> training_file = std::nullopt;
};

struct ParamsGetFineTuningJobs {
    bool print_raw_json = false;
    std::optional<std::string> limit = std::nullopt;
};
