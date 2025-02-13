#pragma once

#include <string>
#include <vector>

namespace models {

struct OpenAIModel {
    int created_at;
    std::string id;
    std::string owned_by;

    void print();
};

void sort(std::vector<OpenAIModel> &models);

} // namespace models
