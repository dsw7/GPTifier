#pragma once

#include <curl/curl.h>
#include <string>

class Curl {
public:
    Curl();
    ~Curl();

    // We want to prevent any copies from being made otherwise we'll attempt
    // to delete a shallow copy of the headers list multiple times (i.e. because the destructor will
    // be called for each copy)
    Curl(const Curl &) = delete;
    Curl &operator=(const Curl &) = delete;

    std::string get_uploaded_files();
    std::string get_models();
    std::string create_chat_completion(const std::string &post_fields);
    std::string create_embedding(const std::string &post_fields);
    std::string upload_file(const std::string &filename, const std::string &purpose);
    std::string delete_file(const std::string &file_id);
    std::string create_fine_tuning_job(const std::string &post_fields);
    std::string delete_model(const std::string &model_id);
    std::string get_fine_tuning_jobs(const std::string &limit);

    CURL *handle = NULL;

private:
    void set_project_id();
    struct curl_slist *headers = NULL;
};
