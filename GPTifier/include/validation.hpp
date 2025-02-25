#pragma once

#include <json.hpp>

namespace validation {

bool is_chat_completion(const nlohmann::json &json);

}
