#include "validation.hpp"

namespace {

bool is_list(const nlohmann::json &json)
{
    return json["object"] == "list";
}

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

} // namespace validation
