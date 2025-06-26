#pragma once

#include <ctime>
#include <string>

namespace networking {
std::string get_costs(const std::time_t &start_time, int limit);
} // namespace networking
