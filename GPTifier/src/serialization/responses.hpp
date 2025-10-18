#pragma once

#include <chrono>
#include <ctime>
#include <json.hpp>
#include <string>
#include <vector>

namespace serialization {

struct Response {
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

struct Responses {
    std::string raw_response;
    std::vector<Response> responses;
};

Response create_response(const std::string &input, const std::string &model, float temp);

} // namespace serialization
