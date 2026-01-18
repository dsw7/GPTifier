#pragma once

#include <chrono>
#include <json.hpp>
#include <string>

namespace serialization {

struct Response {
    int input_tokens = 0;
    int output_tokens = 0;
    std::chrono::duration<float> rtt;
    std::string created;
    std::string input;
    std::string model;
    std::string output;
    std::string raw_response;
    std::string source;
};

Response create_openai_response(const std::string &input, const std::string &model, const float temperature);
Response create_ollama_response(const std::string &prompt, const std::string &model);
std::string test_curl_handle_is_reusable();

} // namespace serialization
