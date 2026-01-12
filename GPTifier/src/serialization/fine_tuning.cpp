#include "fine_tuning.hpp"

#include "api_openai_user.hpp"
#include "ser_utils.hpp"
#include "utils.hpp"

#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

FineTuningJobs unpack_fine_tuning_jobs(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    FineTuningJobs fine_tuning_jobs;
    fine_tuning_jobs.raw_response = response;

    try {
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
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return fine_tuning_jobs;
}

} // namespace

FineTuningJobs get_fine_tuning_jobs(const int limit)
{
    const auto result = networking::get_fine_tuning_jobs(limit);
    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    return unpack_fine_tuning_jobs(result->response);
}

std::string create_fine_tuning_job(const std::string &model, const std::string &training_file)
{
    const nlohmann::json data = { { "model", model }, { "training_file", training_file } };

    const auto result = networking::create_fine_tuning_job(data.dump());
    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    const nlohmann::json json = parse_json(result->response);

    if (not json.contains("id")) {
        throw std::runtime_error("Malformed response. Missing 'id' key");
    }

    return json["id"];
}

} // namespace serialization
