#pragma once

#include <json.hpp>

namespace validation {

bool is_chat_completion(const nlohmann::json &json);
bool is_embedding(const nlohmann::json &json);
bool is_model(const nlohmann::json &json);
bool is_file(const nlohmann::json &json);
bool is_user(const nlohmann::json &json);
bool is_cost(const nlohmann::json &json);
bool is_fine_tuning_job(const nlohmann::json &json);
bool is_embedding_list(const nlohmann::json &json);
bool is_model_list(const nlohmann::json &json);
bool is_file_list(const nlohmann::json &json);
bool is_users_list(const nlohmann::json &json);
bool is_costs_list(const nlohmann::json &json);
bool is_fine_tuning_jobs_list(const nlohmann::json &json);

} // namespace validation
