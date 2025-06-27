#include "fine_tuning.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

void unpack_fine_tuning_jobs(const nlohmann::json &json, FineTuningJobs &fine_tuning_jobs)
{
    for (const auto &entry: json["data"]) {
        FineTuningJob job;

        job.id = entry["id"];
        job.created_at = entry["created_at"];

        if (not entry["finished_at"].is_null()) {
            job.finished_at = utils::datetime_from_unix_timestamp(entry["finished_at"]);
        }

        if (not entry["estimated_finish"].is_null()) {
            job.estimated_finish = utils::datetime_from_unix_timestamp(entry["estimated_finish"]);
        }

        fine_tuning_jobs.jobs.push_back(job);
    }
}

} // namespace

FineTuningJobs get_fine_tuning_jobs(const std::string &limit)
{
    const std::string response = networking::get_fine_tuning_jobs(limit);
    FineTuningJobs fine_tuning_jobs = unpack_response<FineTuningJobs>(response, unpack_fine_tuning_jobs);
    fine_tuning_jobs.raw_response = response;
    return fine_tuning_jobs;
}

std::string create_fine_tuning_job(const std::string &model, const std::string &training_file)
{
    const nlohmann::json data = { { "model", model }, { "training_file", training_file } };
    const std::string response = networking::create_fine_tuning_job(data.dump());
    const nlohmann::json json = response_to_json(response);

    if (not json.contains("id")) {
        throw std::runtime_error("Malformed response. Missing 'id' key");
    }

    return json["id"];
}

} // namespace serialization
