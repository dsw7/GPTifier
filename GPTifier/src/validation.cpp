#include "validation.hpp"

namespace {

bool is_list(const nlohmann::json &json)
{
    return json["object"] == "list";
}

} // namespace

namespace validation {

bool is_chat_completion(const nlohmann::json &json)
{
    return json["object"] == "chat.completion";
}

bool is_embedding(const nlohmann::json &json)
{
    return json["object"] == "embedding";
}

bool is_model(const nlohmann::json &json)
{
    return json["object"] == "model";
}

bool is_file(const nlohmann::json &json)
{
    return json["object"] == "file";
}

bool is_user(const nlohmann::json &json)
{
    return json["object"] == "organization.user";
}

bool is_cost(const nlohmann::json &json)
{
    return json["object"] == "organization.costs.result";
}

bool is_embedding_list(const nlohmann::json &json)
{
    if (not is_list(json)) {
        return false;
    }

    return is_embedding(json);
}

bool is_model_list(const nlohmann::json &json)
{
    if (not is_list(json)) {
        return false;
    }

    return is_model(json);
}

bool is_file_list(const nlohmann::json &json)
{
    if (not is_list(json)) {
        return false;
    }

    return is_file(json);
}

bool is_users_list(const nlohmann::json &json)
{
    if (not is_list(json)) {
        return false;
    }

    return is_user(json);
}

} // namespace validation
