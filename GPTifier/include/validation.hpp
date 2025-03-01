#pragma once

#include <json.hpp>

namespace validation {

void is_page(const nlohmann::json &json);
void is_bucket(const nlohmann::json &json);
void is_list(const nlohmann::json &json);
bool is_list_empty(const nlohmann::json &json);
void is_chat_completion(const nlohmann::json &json);
void is_chat_completion_deleted(const nlohmann::json &json);
void is_embedding(const nlohmann::json &json);
void is_model(const nlohmann::json &json);
void is_file(const nlohmann::json &json);
void is_user(const nlohmann::json &json);
void is_cost(const nlohmann::json &json);
void is_fine_tuning_job(const nlohmann::json &json);
bool is_costs_list(const nlohmann::json &json);

} // namespace validation
