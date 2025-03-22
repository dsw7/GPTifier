#pragma once

#include <optional>
#include <string>

struct Configs {
    void load_configs_from_config_file();

    struct Chat {
        std::optional<std::string> model = std::nullopt;
    };

    struct Embeddings {
        std::optional<std::string> model = std::nullopt;
    };

    Chat chat;
    Embeddings embeddings;
};

extern Configs configs;
