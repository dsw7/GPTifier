#include "fine_tuning.hpp"

#include "api_openai_user.hpp"
#include "ser_utils.hpp"
#include "utils.hpp"

#include <algorithm>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

FineTuningJob unpack_fine_tuning_job_(const nlohmann::json &entry)
{
    FineTuningJob ft_job_obj;

    ft_job_obj.id = entry["id"];

    ft_job_obj.created_at = entry["created_at"];
    ft_job_obj.created_at_dt_str = utils::datetime_from_unix_timestamp(entry["created_at"]);

    if (not entry["finished_at"].is_null()) {
        ft_job_obj.finished_at = utils::datetime_from_unix_timestamp(entry["finished_at"]);
    }

    if (not entry["estimated_finish"].is_null()) {
        ft_job_obj.estimated_finish = utils::datetime_from_unix_timestamp(entry["estimated_finish"]);
    }

    return ft_job_obj;
}

FineTuningJobs unpack_fine_tuning_jobs_(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    FineTuningJobs fine_tuning_jobs;
    fine_tuning_jobs.raw_response = response;

    try {
        for (const auto &entry: json["data"]) {
            fine_tuning_jobs.jobs.push_back(unpack_fine_tuning_job_(entry));
        }
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return fine_tuning_jobs;
}

} // namespace

FineTuningJobs get_fine_tuning_jobs(const int limit_jobs_to_print)
{
    static int min_jobs_to_list = 1;
    static int max_jobs_to_list = 100;

    const int limit_clamped = std::clamp(limit_jobs_to_print, min_jobs_to_list, max_jobs_to_list);

    const auto result = networking::get_fine_tuning_jobs(limit_clamped);

    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    return unpack_fine_tuning_jobs_(result->response);
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
