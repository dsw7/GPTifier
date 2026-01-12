#pragma once

#include <string>
#include <vector>

namespace serialization {

struct Embedding {
    std::string input;
    std::string model;
    std::string source;
    std::vector<float> embedding;
};

Embedding create_openai_embedding(const std::string &model, const std::string &input);
Embedding create_ollama_embedding(const std::string &model, const std::string &input);

} // namespace serialization
