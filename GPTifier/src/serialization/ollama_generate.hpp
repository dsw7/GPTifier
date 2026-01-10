#pragma once

#include <chrono>
#include <ctime>
#include <json.hpp>
#include <string>

namespace serialization {

struct OllamaResponse {
    int input_tokens = 0;
    int output_tokens = 0;
    std::chrono::duration<float> rtt;
    std::string input;
    std::string model;
    std::string output;
    std::string raw_response;
    std::time_t created = 0;
};

OllamaResponse create_ollama_response(const std::string &prompt, const std::string &model);

} // namespace serialization
