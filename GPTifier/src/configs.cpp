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

    this->model_embed = table["command"]["embed"]["model"].value<std::string>();
    this->model_run = table["command"]["run"]["model"].value<std::string>();
    this->model_run_ollama = table["command"]["run"]["model_ollama"].value<std::string>();
    this->model_short = table["command"]["short"]["model"].value<std::string>();
    this->model_short_ollama = table["command"]["short"]["model_ollama"].value<std::string>();
}

Configs configs;
