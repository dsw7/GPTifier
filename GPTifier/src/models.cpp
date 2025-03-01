#include "models.hpp"

#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>

namespace models {

nlohmann::json ChatCompletion::jsonify() const
{
    nlohmann::json results;
    results["completion"] = this->completion;
    results["completion_tokens"] = this->completion_tokens;
    results["created"] = this->created;
    results["id"] = this->id;
    results["model"] = this->model;
    results["prompt"] = this->prompt;
    results["prompt_tokens"] = this->prompt_tokens;

    return results;
}

nlohmann::json Embedding::jsonify() const
{
    nlohmann::json results;
    results["embedding"] = this->embedding;
    results["input"] = this->input;
    results["model"] = this->model;

    return results;
}

void FineTuningJob::print()
{
    const std::string dt_created_at = datetime_from_unix_timestamp(this->created_at);

    std::string finish_time;
    std::string estimated_finish;

    if (this->finished_at.has_value()) {
        finish_time = datetime_from_unix_timestamp(this->finished_at.value());
    } else {
        finish_time = '-';
    }

    if (this->estimated_finish.has_value()) {
        finish_time = datetime_from_unix_timestamp(this->estimated_finish.value());
    } else {
        estimated_finish = '-';
    }

    fmt::print("{:<40}{:<30}{:<30}{}\n", this->id, dt_created_at, estimated_finish, finish_time);
}

void sort(std::vector<FineTuningJob> &jobs)
{
    std::sort(jobs.begin(), jobs.end(), [](const FineTuningJob &left, const FineTuningJob &right) {
        if (left.created_at != right.created_at) {
            return left.created_at < right.created_at;
        }

        return left.id < right.id;
    });
}

} // namespace models
