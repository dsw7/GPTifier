#include "api_ollama.hpp"

#include "configs.hpp"

#include <fmt/core.h>

namespace {

std::string get_ollama_base_url_()
{
    static const std::string host = configs.host_ollama.value();
    static const int port = configs.port_ollama.value();

    static std::string base_url = fmt::format("http://{}:{}/api", host, port);
    return base_url;
}

} // namespace

namespace networking {

CurlResult generate_ollama_response(const std::string &post_fields)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    const std::string url_generate = fmt::format("{}/generate", get_ollama_base_url_());
    curl_easy_setopt(handle, CURLOPT_URL, url_generate.c_str());
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

    const std::string url_embed = fmt::format("{}/embed", get_ollama_base_url_());
    curl_easy_setopt(handle, CURLOPT_URL, url_embed.c_str());
    curl_easy_setopt(handle, CURLOPT_POST, 1L);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

} // namespace networking
