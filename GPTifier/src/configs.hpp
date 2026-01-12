#pragma once

#include <optional>
#include <string>

struct Configs {
    void load_configs_from_config_file();

    std::optional<int> port_ollama;
    std::optional<std::string> host_ollama;
    std::optional<std::string> model_embed_ollama;
    std::optional<std::string> model_embed_openai;
    std::optional<std::string> model_run_ollama;
    std::optional<std::string> model_run_openai;
    std::optional<std::string> model_short_ollama;
    std::optional<std::string> model_short_openai;
};

extern Configs configs;
