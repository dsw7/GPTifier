#pragma once

#include <string>

void query_models_api(std::string &response);
void query_embeddings_api(const std::string &post_fields, std::string &response);
void query_chat_completion_api(const std::string &post_fields, std::string &response);
