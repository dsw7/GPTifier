#pragma once

#include "curl_base.hpp"

#include <ctime>
#include <string>

namespace networking {

class OpenAIAdmin: public CurlBase {
public:
    std::string get_costs(const std::time_t &start_time, int limit);

private:
    void reset_handle();
};

} // namespace networking
