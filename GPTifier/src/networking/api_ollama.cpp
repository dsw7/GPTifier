#include "api_ollama.hpp"

namespace {
// temporarily hard code socket address
const std::string URL_OLLAMA_GENERATE = "http://localhost:11434/api/generate";
const std::string URL_OLLAMA_EMBED = "http://localhost:11434/api/embed";
} // namespace

namespace networking {

CurlResult generate_ollama_response(const std::string &post_fields)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    curl_easy_setopt(handle, CURLOPT_URL, URL_OLLAMA_GENERATE.c_str());
    curl_easy_setopt(handle, CURLOPT_POST, 1L);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

CurlResult create_ollama_embedding(const std::string &post_fields)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    curl_easy_setopt(handle, CURLOPT_URL, URL_OLLAMA_EMBED.c_str());
    curl_easy_setopt(handle, CURLOPT_POST, 1L);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

} // namespace networking
