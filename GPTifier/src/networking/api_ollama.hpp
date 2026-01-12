#pragma once

#include "curl_base.hpp"

#include <string>

namespace networking {
CurlResult generate_ollama_response(const std::string &post_fields);
CurlResult create_ollama_embedding(const std::string &post_fields);
} // namespace networking
