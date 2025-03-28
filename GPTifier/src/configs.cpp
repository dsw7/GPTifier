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

    this->chat.model = table["chat"]["model"].value<std::string>();
    this->embeddings.model = table["embeddings"]["model"].value<std::string>();
}

Configs configs;
