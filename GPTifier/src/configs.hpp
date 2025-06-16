#pragma once

#include <optional>
#include <string>

struct Configs {
    void load_configs_from_config_file();

    std::optional<std::string> model_edit = std::nullopt;
    std::optional<std::string> model_embed = std::nullopt;
    std::optional<std::string> model_run = std::nullopt;
    std::optional<std::string> model_short = std::nullopt;
};

extern Configs configs;
