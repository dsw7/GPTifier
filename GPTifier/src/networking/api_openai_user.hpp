#pragma once

#include "curl_base.hpp"

#include <string>

namespace networking {

std::string get_models();
std::string get_uploaded_files(bool sort_asc = true);
std::string create_chat_completion(const std::string &post_fields);
std::string get_chat_completions(int limit);
std::string delete_chat_completion(const std::string &chat_completion_id);

class OpenAIUser: public CurlBase {
public:
    std::string create_embedding(const std::string &post_fields);
    std::string upload_file(const std::string &filename, const std::string &purpose);
    std::string delete_file(const std::string &file_id);
    std::string create_fine_tuning_job(const std::string &post_fields);
    std::string delete_model(const std::string &model_id);
    std::string get_fine_tuning_jobs(const std::string &limit);

private:
    void reset_handle();
};

} // namespace networking
