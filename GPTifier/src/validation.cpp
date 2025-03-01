#include "validation.hpp"

#include <stdexcept>

namespace {

bool is_page(const nlohmann::json &json)
{
    return json["object"] == "page";
}

bool is_bucket(const nlohmann::json &json)
{
    return json["object"] == "bucket";
}

} // namespace

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

bool is_chat_completion_deleted(const nlohmann::json &json)
{
    return json["object"] == "chat.completion.deleted";
}

bool is_embedding(const nlohmann::json &json)
{
    return json["object"] == "embedding";
}

bool is_model(const nlohmann::json &json)
{
    return json["object"] == "model";
}

void is_file(const nlohmann::json &json)
{
    if (json["object"] != "file") {
        throw std::runtime_error("Object is not a file");
    }
}

bool is_user(const nlohmann::json &json)
{
    return json["object"] == "organization.user";
}

bool is_cost(const nlohmann::json &json)
{
    return json["object"] == "organization.costs.result";
}

bool is_fine_tuning_job(const nlohmann::json &json)
{
    return json["object"] == "fine_tuning.job";
}

bool is_embedding_list(const nlohmann::json &json)
{
    is_list(json);
    return is_embedding(json["data"][0]);
}

bool is_model_list(const nlohmann::json &json)
{
    is_list(json);
    return is_model(json["data"][0]);
}

bool is_users_list(const nlohmann::json &json)
{
    is_list(json);
    return is_user(json["data"][0]);
}

bool is_costs_list(const nlohmann::json &json)
{
    if (not is_page(json)) {
        return false;
    }

    if (not is_bucket(json["data"][0])) {
        return false;
    }

    return is_cost(json["data"][0]["results"][0]);
}

bool is_fine_tuning_jobs_list(const nlohmann::json &json)
{
    is_list(json);
    return is_fine_tuning_job(json["data"][0]);
}

} // namespace validation
