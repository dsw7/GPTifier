#pragma once

#include "curl_connector.hpp"

#include <ctime>
#include <string>

class OpenAIAdmin: public Curl {
public:
    std::string get_costs(const std::time_t &start_time, int limit);
    std::string get_users(int limit = 100);

private:
    void set_admin_key();
};
