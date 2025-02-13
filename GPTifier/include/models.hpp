#pragma once

#include <ctime>
#include <string>
#include <vector>

namespace models {

struct OpenAIModel {
    int created_at;
    std::string id;
    std::string owned_by;

    void print();
};

struct File {
    int created_at;
    std::string filename;
    std::string id;
    std::string purpose;

    void print();
};

struct CostsBucket {
    float cost;
    std::string org_id;
    std::time_t end_time;
    std::time_t start_time;

    void print();
};

void sort(std::vector<OpenAIModel> &models);
void sort(std::vector<File> &files);
void sort(std::vector<CostsBucket> &buckets);

} // namespace models
