#include "params.hpp"

#include <fmt/core.h>
#include <stdexcept>

void ParamsRun::sanitize()
{
    if (std::holds_alternative<float>(this->temperature)) {
        throw std::runtime_error("Temperature is already a float!");
    }

    std::string temperature = std::get<std::string>(this->temperature);

    try {
        this->temperature = std::stof(temperature);
    } catch (std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to float", e.what(), temperature);
        throw std::runtime_error(errmsg);
    }
}
