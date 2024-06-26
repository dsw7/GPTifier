#include "configs.hpp"
#include "utils.hpp"

#include <filesystem>
#include <stdexcept>
#include <toml.hpp>

void Configs::load_configs_from_config_file()
{
    static std::string path_toml = ::get_proj_home_dir() + "/gptifier.toml";

    if (not std::filesystem::exists(path_toml))
    {
        throw std::runtime_error("Could not locate GPTifier configuration file!");
    }

    toml::table table;

    try
    {
        table = toml::parse_file(path_toml);
    }
    catch (const toml::parse_error &e)
    {
        throw std::runtime_error(e);
    }

    this->project_id = table["profile"]["project-id"].value_or("");
    this->chat.model = table["chat"]["model"].value_or("");
    this->embeddings.model = table["embeddings"]["model"].value_or("");
}
