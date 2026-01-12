#pragma once

#include <chrono>
#include <ctime>
#include <json.hpp>
#include <string>

namespace serialization {

struct OpenAIResponse {
    int input_tokens = 0;
    int output_tokens = 0;
    std::chrono::duration<float> rtt;
    std::string id;
    std::string input;
    std::string model;
    std::string output;
    std::string raw_response;
    std::time_t created = 0;
};

struct OllamaResponse {
    int input_tokens = 0;
    int output_tokens = 0;
    std::chrono::duration<float> rtt;
    std::string created;
    std::string input;
    std::string model;
    std::string output;
    std::string raw_response;
};

OpenAIResponse create_openai_response(const std::string &input, const std::string &model, float temp);
OllamaResponse create_ollama_response(const std::string &prompt, const std::string &model);
std::string test_curl_handle_is_reusable();

} // namespace serialization
