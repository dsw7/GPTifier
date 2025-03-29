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
