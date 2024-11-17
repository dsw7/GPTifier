#pragma once

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
    std::string project_id;
};

extern Configs configs;
