#include "models.hpp"

#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>

namespace models {

void OpenAIModel::print()
{
    fmt::print("{:<25}{:<35}{}\n", datetime_from_unix_timestamp(this->created_at), this->owned_by, this->id);
}

void CostsBucket::print()
{
    const std::string dt_start = datetime_from_unix_timestamp(this->start_time);
    const std::string dt_end = datetime_from_unix_timestamp(this->end_time);

    fmt::print("{:<25}{:<25}{:<25}{}\n", dt_start, dt_end, this->cost, this->org_id);
}

void sort(std::vector<OpenAIModel> &models)
{
    std::sort(models.begin(), models.end(), [](const OpenAIModel &left, const OpenAIModel &right) {
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
