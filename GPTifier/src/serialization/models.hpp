#pragma once

#include "ser_utils.hpp"

#include <expected>
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
        return this->created_at < other.created_at;
    }
};

struct Models {
    std::string raw_response;
    std::vector<Model> models;
};

using ModelResult = std::expected<Models, Err>;
using ModelDeleteResult = std::expected<bool, Err>;

ModelResult get_models();
ModelDeleteResult delete_model(const std::string &model_id);

} // namespace serialization
