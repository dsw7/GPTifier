#pragma once

#include <ctime>
#include <string>
#include <vector>

namespace serialization {

struct CostsBucket {
    float cost = 0.00;
    std::string end_time_dt_str;
    std::string org_id;
    std::string start_time_dt_str;
    std::time_t end_time;
    std::time_t start_time;

    bool operator<(const CostsBucket &other) const
    {
        return this->start_time < other.start_time;
    }
};

struct Costs {
    float total_cost = 0.00;
    std::string raw_response;
    std::vector<CostsBucket> buckets;
};

Costs get_costs(const std::time_t start_time, const int limit);

} // namespace serialization
