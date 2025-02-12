#include "params.hpp"

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

bool is_valid_days(int days)
{
    if (days <= 0) {
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

    std::string temperature = std::get<std::string>(this->temperature);

    try {
        this->temperature = std::stof(temperature);
    } catch (std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to float", e.what(), temperature);
        throw std::runtime_error(errmsg);
    }

    if (std::holds_alternative<float>(this->temperature)) {
        if (not is_valid_temp(std::get<float>(this->temperature))) {
            throw std::runtime_error("Temperature must be between 0 and 2");
        }
    }
}

void ParamsCosts::sanitize()
{
    if (std::holds_alternative<int>(this->days)) {
        throw std::runtime_error("Days value is already an int!");
    }

    std::string days = std::get<std::string>(this->days);

    try {
        this->days = std::stoi(days);
    } catch (std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to int", e.what(), days);
        throw std::runtime_error(errmsg);
    }

    if (std::holds_alternative<int>(this->days)) {
        if (not is_valid_days(std::get<int>(this->days))) {
            throw std::runtime_error("Days must be greater than 0");
        }
    }
}
