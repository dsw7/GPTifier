#pragma once

#include "curl_connector.hpp"

#include <ctime>
#include <curl/curl.h>
#include <string>

class OpenAI: public Curl {
public:
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

private:
    void set_admin_key();
    void set_api_key();
    void set_project_id();
};
