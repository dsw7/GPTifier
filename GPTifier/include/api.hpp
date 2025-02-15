#pragma once

#include <ctime>
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

    // Admin commands
    std::string get_costs(const std::time_t &start_time, int limit);
    std::string get_users(int limit = 100);

    // User commands
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
    struct curl_slist *headers = NULL;

    void set_admin_key();
    void set_api_key();
    void set_project_id();
    void set_content_type_transmit_json();
    void set_content_type_submit_form();
};
