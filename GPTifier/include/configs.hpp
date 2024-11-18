#pragma once

#include <optional>
#include <string>

struct Configs {
    void load_configs_from_config_file();

    struct Chat {
        std::string model;
    };

    struct Embeddings {
        std::string model;
    };

    Chat chat;
    Embeddings embeddings;
    std::optional<std::string> project_id = std::nullopt;
};

extern Configs configs;
