#include "serialization/fine_tuning.hpp"

#include "networking/api_openai_user.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

void unpack_fine_tuning_jobs(const nlohmann::json &results, FineTuningJobs &fine_tuning_jobs)
{
    for (const auto &entry: results["data"]) {
        FineTuningJob job;

        job.id = entry["id"];
        job.created_at = entry["created_at"];

        if (not entry["finished_at"].is_null()) {
            job.finished_at = datetime_from_unix_timestamp(entry["finished_at"]);
        }

        if (not entry["estimated_finish"].is_null()) {
            job.estimated_finish = datetime_from_unix_timestamp(entry["estimated_finish"]);
        }

        fine_tuning_jobs.jobs.push_back(job);
    }
}

FineTuningJobs unpack_response(const std::string &response)
{
    FineTuningJobs fine_tuning_jobs;
    nlohmann::json results;

    try {
        results = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    try {
        unpack_fine_tuning_jobs(results, fine_tuning_jobs);
    } catch (nlohmann::json::out_of_range &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    } catch (nlohmann::json::type_error &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return fine_tuning_jobs;
}

} // namespace

FineTuningJobs get_fine_tuning_jobs(const std::string &limit)
{
    OpenAIUser api;
    const std::string response = api.get_fine_tuning_jobs(limit);

    FineTuningJobs fine_tuning_jobs = unpack_response(response);
    fine_tuning_jobs.raw_response = response;
    return fine_tuning_jobs;
}

std::string create_fine_tuning_job(const std::string &model, const std::string &training_file)
{
    const nlohmann::json data = { { "model", model }, { "training_file", training_file } };

    OpenAIUser api;
    const std::string response = api.create_fine_tuning_job(data.dump());

    nlohmann::json results;
    try {
        results = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    if (not results.contains("id")) {
        throw std::runtime_error("Malformed response. Missing 'id' key");
    }

    return results["id"];
}
