#pragma once

#include <string>

std::string query_models_api();
std::string query_embeddings_api(const std::string &post_fields);
std::string query_chat_completion_api(const std::string &post_fields);
