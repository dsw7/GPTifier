#pragma once

#include <string>
#include <vector>

struct FineTuningJob {
    int created_at;
    std::string estimated_finish = "-";
    std::string finished_at = "-";
    std::string id;
};

struct FineTuningJobs {
    std::string raw_response;
    std::vector<FineTuningJob> jobs;
};

std::string create_fine_tuning_job(const std::string &model, const std::string &training_file);
FineTuningJobs get_fine_tuning_jobs(const std::string &limit);
