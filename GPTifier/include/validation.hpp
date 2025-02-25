#pragma once

#include <json.hpp>

namespace validation {

bool is_chat_completion(const nlohmann::json &json);
bool is_embedding(const nlohmann::json &json);
bool is_model_list(const nlohmann::json &json);
bool is_file_list(const nlohmann::json &json);

} // namespace validation
