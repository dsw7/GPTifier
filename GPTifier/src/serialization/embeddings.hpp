#pragma once

#include <string>
#include <vector>

namespace serialization {

struct Embedding {
    std::string input;
    std::string model;
    std::vector<float> embedding;
};

Embedding create_embedding(const std::string &model, const std::string &input);

} // namespace serialization
