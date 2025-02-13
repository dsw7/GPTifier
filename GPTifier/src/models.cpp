#include "models.hpp"

#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>

namespace models {

void OpenAIModel::print()
{
    fmt::print("{:<25}{:<35}{}\n", datetime_from_unix_timestamp(this->created_at), this->owned_by, this->id);
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

} // namespace models
