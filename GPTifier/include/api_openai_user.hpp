#pragma once

#include "curl_connector.hpp"

#include <string>

class OpenAIUser: public Curl {
public:
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
    void set_api_key();
    void set_project_id();
};
