#pragma once

#include <json.hpp>

namespace validation {

void is_chat_completion(const nlohmann::json &json);

}
