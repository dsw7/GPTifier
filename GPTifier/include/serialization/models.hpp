#pragma once

#include <string>
#include <vector>

struct Model {
    bool owned_by_openai = false;
    int created_at;
    std::string id;
    std::string owner;
};

struct Models {
    std::string raw_response;
    std::vector<Model> models;
};

Models get_models();
bool delete_model(const std::string &model_id);
