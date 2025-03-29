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
