#include "interface/params.hpp"

#include "utils.hpp"

#include <fmt/core.h>
#include <stdexcept>

namespace {

bool is_valid_temp(float temperature)
{
    if (temperature < 0 || temperature > 2) {
        return false;
    }

    return true;
}

bool is_valid_limit(int limit)
{
    return limit > 0;
}

} // namespace

void ParamsRun::sanitize()
{
    if (std::holds_alternative<float>(this->temperature)) {
        throw std::runtime_error("Temperature is already a float!");
    }

    const std::string temperature = std::get<std::string>(this->temperature);
    this->temperature = utils::string_to_float(temperature);

    if (std::holds_alternative<float>(this->temperature)) {
        if (not is_valid_temp(std::get<float>(this->temperature))) {
            throw std::runtime_error("Temperature must be between 0 and 2");
        }
    }
}

void ParamsGetChatCompletions::sanitize()
{
    if (std::holds_alternative<int>(this->limit)) {
        throw std::runtime_error("Limit value is already an int!");
    }

    std::string limit = std::get<std::string>(this->limit);
    this->limit = utils::string_to_int(limit);

    if (std::holds_alternative<int>(this->limit)) {
        if (not is_valid_limit(std::get<int>(this->limit))) {
            throw std::runtime_error("Limit must be greater than 0");
        }
    }
}
