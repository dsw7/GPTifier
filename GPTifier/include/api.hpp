#pragma once

#include <string>

namespace api {
std::string create_chat_completion(const std::string &model, const std::string &prompt, float temperature);
std::string create_embedding(const std::string &model, const std::string &input);
std::string get_uploaded_files();
std::string get_models();
std::string upload_file(const std::string &filename, const std::string &purpose);
std::string delete_file(const std::string &file_id);
std::string create_fine_tuning_job(const std::string &training_file, const std::string &model);
std::string delete_model(const std::string &model_id);
} // namespace api
