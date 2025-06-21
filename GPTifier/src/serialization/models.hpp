#pragma once

#include <string>
#include <vector>

namespace serialization {

struct Model {
    bool owned_by_openai = false;
    int created_at;
    std::string id;
    std::string owner;

    bool operator<(const Model &other) const
    {
        return created_at < other.created_at;
    }
};

struct Models {
    std::string raw_response;
    std::vector<Model> models;
};

Models get_models();
bool delete_model(const std::string &model_id);

} // namespace serialization
