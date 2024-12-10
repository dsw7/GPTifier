#include "api.hpp"
#include "configs.hpp"

#include <cstdlib>
#include <curl/curl.h>
#include <optional>
#include <stdexcept>
#include <string>

namespace endpoints {

const std::string URL_CHAT_COMPLETIONS = "https://api.openai.com/v1/chat/completions";
const std::string URL_EMBEDDINGS = "https://api.openai.com/v1/embeddings";
const std::string URL_FILES = "https://api.openai.com/v1/files";
const std::string URL_MODELS = "https://api.openai.com/v1/models";

} // namespace endpoints

namespace {

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

std::optional<std::string> get_api_key()
{
    const char *api_key = std::getenv("OPENAI_API_KEY");

    if (api_key) {
        return std::string(api_key);
    }

    return std::nullopt;
}

void catch_curl_error(CURLcode code)
{
    if (code != CURLE_OK) {
        const std::string errmsg = "Failed to run query. " + std::string(curl_easy_strerror(code));
        throw std::runtime_error(errmsg);
    }
}

class Curl {
public:
    Curl();
    ~Curl();
    std::string get_models();
    std::string get_files();
    std::string post(const std::string &endpoint, const std::string &post_fields);
    std::string upload_file(const std::string &endpoint, const std::string &filename, const std::string &purpose);

    CURL *handle = NULL;

private:
    struct curl_slist *headers = NULL;
};

Curl::Curl()
{
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    const std::optional<std::string> api_key = get_api_key();

    if (not api_key.has_value()) {
        throw std::runtime_error("OPENAI_API_KEY environment variable not set");
    }

    this->handle = curl_easy_init();

    if (this->handle == NULL) {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    const std::string header_auth = "Authorization: Bearer " + api_key.value();

    this->headers = curl_slist_append(this->headers, header_auth.c_str());

    if (configs.project_id.has_value()) {
        const std::string header_project_id = "OpenAI-Project: " + configs.project_id.value();
        this->headers = curl_slist_append(this->headers, header_project_id.c_str());
    }

    this->headers = curl_slist_append(this->headers, "Content-Type: application/json");
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_WRITEFUNCTION, write_callback);
}

Curl::~Curl()
{
    if (this->handle) {
        curl_slist_free_all(this->headers);
        curl_easy_cleanup(this->handle);
    }

    curl_global_cleanup();
}

std::string Curl::get_models()
{
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_MODELS.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    const CURLcode rv = curl_easy_perform(this->handle);
    catch_curl_error(rv);

    return response;
}

std::string Curl::get_files()
{
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_FILES.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    const CURLcode rv = curl_easy_perform(this->handle);
    catch_curl_error(rv);

    return response;
}

std::string Curl::post(const std::string &endpoint, const std::string &post_fields)
{
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    const CURLcode rv = curl_easy_perform(this->handle);
    catch_curl_error(rv);

    return response;
}

std::string Curl::upload_file(const std::string &endpoint, const std::string &filename, const std::string &purpose)
{
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_mime *form = NULL;
    form = curl_mime_init(this->handle);

    curl_mimepart *field = NULL;

    field = curl_mime_addpart(form);
    curl_mime_name(field, "purpose");
    curl_mime_data(field, purpose.c_str(), CURL_ZERO_TERMINATED);

    field = curl_mime_addpart(form);
    curl_mime_name(field, "file");
    curl_mime_filedata(field, filename.c_str());

    curl_easy_setopt(this->handle, CURLOPT_MIMEPOST, form);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    const CURLcode rv = curl_easy_perform(this->handle);
    curl_mime_free(form);

    catch_curl_error(rv);
    return response;
}

} // namespace

std::string query_models_api()
{
    Curl curl;
    return curl.get_models();
}

std::string query_embeddings_api(const std::string &post_fields)
{
    Curl curl;
    return curl.post(endpoints::URL_EMBEDDINGS, post_fields);
}

std::string query_list_files_api()
{
    Curl curl;
    return curl.get_files();
}

std::string query_upload_file_api(const std::string &filename)
{
    Curl curl;
    const std::string purpose = "fine-tune";
    return curl.upload_file(endpoints::URL_FILES, filename, purpose);
}

std::string query_chat_completion_api(const std::string &post_fields)
{
    Curl curl;
    return curl.post(endpoints::URL_CHAT_COMPLETIONS, post_fields);
}
