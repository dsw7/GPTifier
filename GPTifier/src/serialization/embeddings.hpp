#pragma once

#include "ser_utils.hpp"

#include <expected>
#include <string>
#include <vector>

namespace serialization {

struct Embedding {
    std::string input;
    std::string model;
    std::vector<float> embedding;
};

using EmbedResult = std::expected<Embedding, Err>;
EmbedResult create_embedding(const std::string &model, const std::string &input);

} // namespace serialization
