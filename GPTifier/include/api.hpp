#pragma once

#include <string>

std::string query_chat_completion_api(const std::string &model, const std::string &prompt, float temperature);
std::string query_embeddings_api(const std::string &model, const std::string &input);
std::string query_list_files_api();
std::string query_models_api();
std::string query_upload_file_api(const std::string &filename, const std::string &purpose);
std::string query_delete_file_api(const std::string &file_id);
std::string query_create_fine_tuning_job_api(const std::string &training_file, const std::string &model);
std::string query_delete_model(const std::string &model_id);
