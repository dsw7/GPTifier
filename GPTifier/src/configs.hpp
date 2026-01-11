#pragma once

#include <optional>
#include <string>

struct Configs {
    void load_configs_from_config_file();

    std::optional<std::string> model_embed;
    std::optional<std::string> model_run;
    std::optional<std::string> model_run_ollama;
    std::optional<std::string> model_short;
    std::optional<std::string> model_short_ollama;
};

extern Configs configs;
