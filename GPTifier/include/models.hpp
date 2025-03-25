#pragma once

#include <string>

namespace models {

struct FineTuningJob {
    int created_at;
    std::string estimated_finish = "-";
    std::string finished_at = "-";
    std::string id;
};

} // namespace models
