#include "interface/model_selector.hpp"

#include "configs.hpp"

#include <stdexcept>

std::string select_chat_model()
{
#ifdef TESTING_ENABLED
    static std::string low_cost_model = "gpt-3.5-turbo";
    return low_cost_model;
#endif

    if (configs.chat.model.has_value()) {
        return configs.chat.model.value();
    }

    throw std::runtime_error("Could not determine which model to use");
}
