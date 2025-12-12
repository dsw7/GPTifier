#pragma once

#include "curl_base.hpp"

#include <string>

namespace networking {
CurlResult get_models();
CurlResult delete_model(const std::string &model_id);
CurlResult create_response(const std::string &post_fields);
CurlResult create_embedding(const std::string &post_fields);
std::string upload_file(const std::string &filename, const std::string &purpose);
std::string get_uploaded_files(bool sort_asc = true);
std::string delete_file(const std::string &file_id);
std::string create_fine_tuning_job(const std::string &post_fields);
std::string get_fine_tuning_jobs(const int limit);
CurlResult create_image(const std::string &post_fields);
} // namespace networking
