#include "configs.hpp"
#include "datadir.hpp"
#include "toml.hpp"

#include <filesystem>
#include <stdexcept>

void Configs::load_configs_from_config_file()
{
    if (not std::filesystem::exists(datadir::GPT_CONFIG)) {
        throw std::runtime_error("Could not locate GPTifier configuration file!");
    }

    toml::table table;

    try {
        table = toml::parse_file(datadir::GPT_CONFIG.string());
    } catch (const toml::parse_error &e) {
        throw std::runtime_error(e);
    }

    // ollama
    this->host_ollama = table["ollama"]["host"].value_or<std::string>("localhost");
    this->port_ollama = table["ollama"]["port"].value_or<int>(11434);

    // run command
    this->model_run_openai = table["command"]["run"]["model"].value<std::string>();
    this->model_run_ollama = table["command"]["run"]["model_ollama"].value<std::string>();

    // short command
    this->model_short_openai = table["command"]["short"]["model"].value<std::string>();
    this->model_short_ollama = table["command"]["short"]["model_ollama"].value<std::string>();

    // embed command
    this->model_embed_openai = table["command"]["embed"]["model"].value_or<std::string>("text-embedding-3-small");
    this->model_embed_ollama = table["command"]["embed"]["model_ollama"].value_or<std::string>("embeddinggemma");
}

Configs configs;
