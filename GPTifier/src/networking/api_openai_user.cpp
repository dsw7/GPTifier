#include "api_openai_user.hpp"

#include <cstdlib>
#include <expected>
#include <fmt/core.h>
#include <stdexcept>

namespace {

const std::string URL_EMBEDDINGS = "https://api.openai.com/v1/embeddings";
const std::string URL_FILES = "https://api.openai.com/v1/files";
const std::string URL_FINE_TUNING = "https://api.openai.com/v1/fine_tuning";
const std::string URL_IMAGES = "https://api.openai.com/v1/images";
const std::string URL_MODELS = "https://api.openai.com/v1/models";
const std::string URL_RESPONSES = "https://api.openai.com/v1/responses";

std::string get_user_api_key()
{
    static std::string api_key;

    if (api_key.empty()) {
        const char *env_api_key = std::getenv("OPENAI_API_KEY");

        if (env_api_key == nullptr) {
            throw std::runtime_error("OPENAI_API_KEY environment variable not set");
        }

        api_key = env_api_key;
    }
    return api_key;
}

} // namespace

namespace networking {

CurlResult get_models()
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    curl_easy_setopt(handle, CURLOPT_URL, URL_MODELS.c_str());
    curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

CurlResult delete_model(const std::string &model_id)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    const std::string endpoint = fmt::format("{}/{}", URL_MODELS, model_id);
    curl_easy_setopt(handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

CurlResult create_response(const std::string &post_fields)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl.append_header("Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    curl_easy_setopt(handle, CURLOPT_URL, URL_RESPONSES.c_str());
    curl_easy_setopt(handle, CURLOPT_POST, 1L);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

CurlResult create_embedding(const std::string &post_fields)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl.append_header("Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    curl_easy_setopt(handle, CURLOPT_URL, URL_EMBEDDINGS.c_str());
    curl_easy_setopt(handle, CURLOPT_POST, 1L);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

CurlResult upload_file(const std::string &filename, const std::string &purpose)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl.append_header("Content-Type: multipart/form-data");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    curl_easy_setopt(handle, CURLOPT_URL, URL_FILES.c_str());

    curl_mime *form = curl_mime_init(handle);
    curl_mimepart *field = NULL;

    field = curl_mime_addpart(form);
    curl_mime_name(field, "purpose");
    curl_mime_data(field, purpose.c_str(), CURL_ZERO_TERMINATED);

    field = curl_mime_addpart(form);
    curl_mime_name(field, "file");
    curl_mime_filedata(field, filename.c_str());

    curl_easy_setopt(handle, CURLOPT_MIMEPOST, form);

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    curl_mime_free(form);
    return check_curl_code(handle, code, response);
}

CurlResult get_uploaded_files(bool sort_asc)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    const std::string order = sort_asc ? "asc" : "desc";
    const std::string endpoint = fmt::format("{}?order={}", URL_FILES, order);
    curl_easy_setopt(handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

CurlResult delete_file(const std::string &file_id)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    const std::string endpoint = fmt::format("{}/{}", URL_FILES, file_id);
    curl_easy_setopt(handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

std::string create_fine_tuning_job(const std::string &post_fields)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl.append_header("Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    const std::string endpoint = fmt::format("{}/{}", URL_FINE_TUNING, "jobs");
    curl_easy_setopt(handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(handle, CURLOPT_POST, 1L);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    if (code != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(code));
    }
    return response;
}

std::string get_fine_tuning_jobs(const int limit)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    const std::string endpoint = fmt::format("{}/{}?limit={}", URL_FINE_TUNING, "jobs", limit);
    curl_easy_setopt(handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    if (code != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(code));
    }
    return response;
}

CurlResult create_image(const std::string &post_fields)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl.append_header("Authorization: Bearer " + get_user_api_key());
    curl.append_header("Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curl.get_headers());

    const std::string endpoint = fmt::format("{}/{}", URL_IMAGES, "generations");
    curl_easy_setopt(handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(handle, CURLOPT_POST, 1L);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);

    const CURLcode code = curl_easy_perform(handle);
    return check_curl_code(handle, code, response);
}

} // namespace networking
