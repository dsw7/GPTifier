#pragma once

#include "curl_base.hpp"

#include <ctime>

namespace networking {
CurlResult get_costs(const std::time_t start_time, const int limit);
} // namespace networking
