#pragma once

#include <ctime>
#include <json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace models {

struct Completion {
    int completion_tokens = 0;
    int prompt_tokens = 0;
    std::string completion;
    std::string model;
    std::string prompt;
    std::time_t created = 0;

    nlohmann::json jsonify() const;
};

struct Embedding {
    std::string input;
    std::string model;
    std::vector<float> embedding;

    nlohmann::json jsonify() const;
};

struct Model {
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

struct Job {
    int created_at;
    std::optional<int> estimated_finish = std::nullopt;
    std::optional<int> finished_at = std::nullopt;
    std::string id;

    void print();
};

struct CostsBucket {
    float cost;
    std::string org_id;
    std::time_t end_time;
    std::time_t start_time;

    void print();
};

void sort(std::vector<Model> &models);
void sort(std::vector<File> &files);
void sort(std::vector<Job> &jobs);
void sort(std::vector<CostsBucket> &buckets);

} // namespace models
