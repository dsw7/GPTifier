#include "models.hpp"

#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>

namespace models {

nlohmann::json Completion::jsonify() const
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

void Completion::print()
{
    const std::string dt_created = datetime_from_unix_timestamp(this->created);
    fmt::print("{:<25}{:<40}{:<35}{}\n", dt_created, this->id, this->prompt, this->completion);
}

nlohmann::json Embedding::jsonify() const
{
    nlohmann::json results;
    results["embedding"] = this->embedding;
    results["input"] = this->input;
    results["model"] = this->model;

    return results;
}

void Job::print()
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

void CostsBucket::print()
{
    const std::string dt_start = datetime_from_unix_timestamp(this->start_time);
    const std::string dt_end = datetime_from_unix_timestamp(this->end_time);

    fmt::print("{:<25}{:<25}{:<25}{}\n", dt_start, dt_end, this->cost, this->org_id);
}

void sort(std::vector<Model> &models)
{
    std::sort(models.begin(), models.end(), [](const Model &left, const Model &right) {
        if (left.created_at != right.created_at) {
            return left.created_at < right.created_at;
        }

        return left.id < right.id;
    });
}

void sort(std::vector<File> &files)
{
    std::sort(files.begin(), files.end(), [](const File &left, const File &right) {
        if (left.created_at != right.created_at) {
            return left.created_at < right.created_at;
        }

        return left.id < right.id;
    });
}

void sort(std::vector<Job> &jobs)
{
    std::sort(jobs.begin(), jobs.end(), [](const Job &left, const Job &right) {
        if (left.created_at != right.created_at) {
            return left.created_at < right.created_at;
        }

        return left.id < right.id;
    });
}

void sort(std::vector<CostsBucket> &buckets)
{
    std::sort(buckets.begin(), buckets.end(), [](const CostsBucket &left, const CostsBucket &right) {
        if (left.start_time != right.start_time) {
            return left.start_time < right.start_time;
        }

        return left.org_id < right.org_id;
    });
}

} // namespace models
