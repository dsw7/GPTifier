#pragma once

#include <json.hpp>

namespace validation {

void is_list(const nlohmann::json &json);
bool is_list_empty(const nlohmann::json &json);
void is_chat_completion(const nlohmann::json &json);
void is_chat_completion_deleted(const nlohmann::json &json);

} // namespace validation
