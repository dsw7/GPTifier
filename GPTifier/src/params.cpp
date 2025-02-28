#include "params.hpp"

#include <fmt/core.h>
#include <stdexcept>

namespace {

float parse_temp(const std::string &temperature)
{
    float temp_f = 0.00;

    try {
        temp_f = std::stof(temperature);
    } catch (std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to float", e.what(), temperature);
        throw std::runtime_error(errmsg);
    }

    return temp_f;
}

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
    this->temperature = parse_temp(temperature);

    if (std::holds_alternative<float>(this->temperature)) {
        if (not is_valid_temp(std::get<float>(this->temperature))) {
            throw std::runtime_error("Temperature must be between 0 and 2");
        }
    }
}

void ParamsShort::sanitize()
{
    if (not this->prompt.has_value()) {
        throw std::runtime_error("Prompt is empty");
    }

    if (std::holds_alternative<float>(this->temperature)) {
        throw std::runtime_error("Temperature is already a float!");
    }

    const std::string temperature = std::get<std::string>(this->temperature);
    this->temperature = parse_temp(temperature);

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

void ParamsGetChatCompletions::sanitize()
{
    if (std::holds_alternative<int>(this->limit)) {
        throw std::runtime_error("Limit value is already an int!");
    }

    std::string limit = std::get<std::string>(this->limit);

    try {
        this->limit = std::stoi(limit);
    } catch (std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to int", e.what(), limit);
        throw std::runtime_error(errmsg);
    }

    if (std::holds_alternative<int>(this->limit)) {
        if (not is_valid_limit(std::get<int>(this->limit))) {
            throw std::runtime_error("Limit must be greater than 0");
        }
    }
}
