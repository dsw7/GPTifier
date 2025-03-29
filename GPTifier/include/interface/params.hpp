#pragma once

#include <optional>
#include <string>
#include <variant>

struct ParamsRun {
    bool enable_export = true;
    bool store_completion = false;
    std::optional<std::string> json_dump_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> prompt = std::nullopt;
    std::optional<std::string> prompt_file = std::nullopt;
    std::variant<std::string, float> temperature = "1.00";

    void sanitize();
};

struct ParamsFineTune {
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> training_file = std::nullopt;
};

struct ParamsGetFineTuningJobs {
    bool print_raw_json = false;
    std::optional<std::string> limit = std::nullopt;
};

struct ParamsCosts {
    bool print_raw_json = false;
    std::variant<std::string, int> days = "30";

    void sanitize();
};

struct ParamsGetChatCompletions {
    bool print_raw_json = false;
    std::variant<std::string, int> limit = "20";

    void sanitize();
};
