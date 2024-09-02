#include "data_dir.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <stdexcept>
#include <stdlib.h>

namespace
{

std::string get_proj_data_dir()
{
    const char *home_dir = std::getenv("HOME");

    if (not home_dir)
    {
        throw std::runtime_error("Could not locate user home directory!");
    }

    std::string data_dir = std::string(home_dir) + "/.gptifier";

    if (not std::filesystem::exists(data_dir))
    {
        std::string errmsg = fmt::format("Could not locate '{}'", data_dir);
        throw std::runtime_error(errmsg);
    }

    return data_dir;
}

} // namespace

namespace datadir
{

const std::string GPT_DATADIR = get_proj_data_dir();
const std::string GPT_CONFIG = fmt::format("{}/gptifier.toml", GPT_DATADIR);
const std::string GPT_COMPLETIONS = fmt::format("{}/completions.gpt", GPT_DATADIR);
const std::string GPT_EMBEDDINGS = fmt::format("{}/embeddings.gpt", GPT_DATADIR);

} // namespace datadir
