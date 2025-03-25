#include "serialization/validation.hpp"

#include <stdexcept>

namespace validation {

void is_page(const nlohmann::json &json)
{
    if (json["object"] != "page") {
        throw std::runtime_error("Object is not a page");
    }
}

void is_bucket(const nlohmann::json &json)
{
    if (json["object"] != "bucket") {
        throw std::runtime_error("Object is not a bucket");
    }
}

bool is_bucket_empty(const nlohmann::json &json)
{
    return json["results"].empty();
}

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

void is_embedding(const nlohmann::json &json)
{
    if (json["object"] != "embedding") {
        throw std::runtime_error("Object is not an embedding");
    }
}

void is_model(const nlohmann::json &json)
{
    if (json["object"] != "model") {
        throw std::runtime_error("Object is not a model");
    }
}

void is_file(const nlohmann::json &json)
{
    if (json["object"] != "file") {
        throw std::runtime_error("Object is not a file");
    }
}

void is_user(const nlohmann::json &json)
{
    if (json["object"] != "organization.user") {
        throw std::runtime_error("Object is not an organization.user object");
    }
}

void is_cost(const nlohmann::json &json)
{
    if (json["object"] != "organization.costs.result") {
        throw std::runtime_error("Object is not an organization.costs.result object");
    }
}

void is_fine_tuning_job(const nlohmann::json &json)
{
    if (json["object"] != "fine_tuning.job") {
        throw std::runtime_error("Object is not an fine_tuning.job object");
    }
}

} // namespace validation
