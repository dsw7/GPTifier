#pragma once

#include <chrono>
#include <ctime>
#include <json.hpp>
#include <string>
#include <vector>

namespace serialization {

struct ChatCompletion {
    int completion_tokens = 0;
    int prompt_tokens = 0;
    std::chrono::duration<float> rtt;
    std::string completion;
    std::string id;
    std::string model;
    std::string prompt;
    std::string raw_response;
    std::time_t created = 0;
};

struct ChatCompletions {
    std::string raw_response;
    std::vector<ChatCompletion> completions;
};

ChatCompletion create_chat_completion(const std::string &prompt, const std::string &model, float temp);

} // namespace serialization
