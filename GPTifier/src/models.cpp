#include "models.hpp"

#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>

namespace models {

void OpenAIModel::print()
{
    const std::string dt_created_at = datetime_from_unix_timestamp(this->created_at);
    fmt::print("{:<25}{:<35}{}\n", dt_created_at, this->owned_by, this->id);
}

void File::print()
{
    const std::string dt_created_at = datetime_from_unix_timestamp(this->created_at);
    fmt::print("{:<30}{:<30}{:<30}{}\n", this->id, this->filename, dt_created_at, this->purpose);
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

void sort(std::vector<File> &files)
{
    std::sort(files.begin(), files.end(), [](const File &left, const File &right) {
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
