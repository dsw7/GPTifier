#pragma once

#include <ctime>
#include <string>

namespace serialization {

struct Image {
    int input_tokens_image = 0;
    int input_tokens_text = 0;
    int output_tokens = 0;
    std::string b64_json;
    std::time_t created = 0;
};

Image create_image(const std::string &model, const std::string &prompt, const std::string &quality, const std::string &style);
} // namespace serialization
