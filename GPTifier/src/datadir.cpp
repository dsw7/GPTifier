#include "datadir.hpp"

#include <fmt/core.h>
#include <stdlib.h>

namespace {

std::filesystem::path get_proj_data_dir()
{
    const char *home_dir = std::getenv("HOME");

    if (not home_dir) {
        fmt::print(stderr, "Could not locate user home directory!\n");
        exit(EXIT_FAILURE);
    }

    const std::string data_dir = std::string(home_dir) + "/.gptifier";

    if (not std::filesystem::exists(data_dir)) {
        fmt::print(stderr, "Could not locate '{}'\n", data_dir);
        exit(EXIT_FAILURE);
    }

    return std::filesystem::path(data_dir);
}

} // namespace

namespace datadir {

const std::filesystem::path GPT_DATADIR = get_proj_data_dir();
const std::filesystem::path GPT_CONFIG = GPT_DATADIR / "gptifier.toml";
const std::filesystem::path GPT_COMPLETIONS = GPT_DATADIR / "completions.gpt";
const std::filesystem::path GPT_EMBEDDINGS = GPT_DATADIR / "embeddings.gpt";

} // namespace datadir
