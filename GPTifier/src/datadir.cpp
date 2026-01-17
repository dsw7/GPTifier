#include "datadir.hpp"

#include <fmt/core.h>
#include <stdlib.h>

namespace {

namespace fs = std::filesystem;

fs::path get_project_data_dir_()
{
    const char *home_dir = std::getenv("HOME");

    if (not home_dir) {
        fmt::print(stderr, "Could not locate user home directory!\n");
        exit(EXIT_FAILURE);
    }

    const fs::path project_data_dir = fs::path(home_dir) / ".gptifier";

    if (not fs::exists(project_data_dir)) {
        fmt::print(stderr, "Could not locate '{}'\n", project_data_dir.string());
        exit(EXIT_FAILURE);
    }

    return project_data_dir;
}

} // namespace

namespace datadir {

const fs::path GPT_DATADIR = get_project_data_dir_();
const fs::path GPT_CONFIG = GPT_DATADIR / "gptifier.toml";
const fs::path GPT_COMPLETIONS = GPT_DATADIR / "completions.gpt";
const fs::path GPT_EMBEDDINGS = GPT_DATADIR / "embeddings.gpt";

} // namespace datadir
