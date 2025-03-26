#include "serialization/validation.hpp"

#include <stdexcept>

namespace validation {

void is_list(const nlohmann::json &json)
{
    if (json["object"] != "list") {
        throw std::runtime_error("Object is not a list");
    }
}

bool is_list_empty(const nlohmann::json &json)
{
    return json["data"].empty();
}

void is_chat_completion(const nlohmann::json &json)
{
    if (json["object"] != "chat.completion") {
        throw std::runtime_error("Object is not a chat completion");
    }
}

void is_chat_completion_deleted(const nlohmann::json &json)
{
    if (json["object"] != "chat.completion.deleted") {
        throw std::runtime_error("Object is not a chat completion deletion");
    }
}

void is_model(const nlohmann::json &json)
{
    if (json["object"] != "model") {
        throw std::runtime_error("Object is not a model");
    }
}

void is_user(const nlohmann::json &json)
{
    if (json["object"] != "organization.user") {
        throw std::runtime_error("Object is not an organization.user object");
    }
}

void is_fine_tuning_job(const nlohmann::json &json)
{
    if (json["object"] != "fine_tuning.job") {
        throw std::runtime_error("Object is not an fine_tuning.job object");
    }
}

} // namespace validation
