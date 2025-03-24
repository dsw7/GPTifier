#pragma once

#include <chrono>
#include <ctime>
#include <json.hpp>
#include <string>
#include <vector>

namespace models {

struct ChatCompletion {
    int completion_tokens = 0;
    int prompt_tokens = 0;
    std::chrono::duration<float> rtt;
    std::string completion;
    std::string id;
    std::string model;
    std::string prompt;
    std::time_t created = 0;

    nlohmann::json jsonify() const;
};

struct Model {
    int created_at;
    std::string id;
    std::string owned_by;
};

struct File {
    int created_at;
    std::string filename;
    std::string id;
    std::string purpose;
};

struct FineTuningJob {
    int created_at;
    std::string estimated_finish = "-";
    std::string finished_at = "-";
    std::string id;
};

struct CostsBucket {
    float cost;
    std::string org_id;
    std::time_t end_time;
    std::time_t start_time;
};

} // namespace models
