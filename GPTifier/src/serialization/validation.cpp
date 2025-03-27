#include "serialization/validation.hpp"

#include <stdexcept>

namespace validation {

void is_list(const nlohmann::json &json)
{
    if (json["object"] != "list") {
        throw std::runtime_error("Object is not a list");
    }
}

void is_chat_completion(const nlohmann::json &json)
{
    if (json["object"] != "chat.completion") {
        throw std::runtime_error("Object is not a chat completion");
    }
}

} // namespace validation
