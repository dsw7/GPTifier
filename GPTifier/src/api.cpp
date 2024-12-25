#include "api.hpp"
#include "configs.hpp"

#include <cstdlib>
#include <curl/curl.h>
#include <fmt/core.h>
#include <json.hpp>
#include <optional>
#include <stdexcept>

namespace endpoints {

const std::string URL_CHAT_COMPLETIONS = "https://api.openai.com/v1/chat/completions";
const std::string URL_EMBEDDINGS = "https://api.openai.com/v1/embeddings";
const std::string URL_FILES = "https://api.openai.com/v1/files";
const std::string URL_FINE_TUNING = "https://api.openai.com/v1/fine_tuning";
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
    std::string get_files();
    std::string get_models();
    std::string post_chat_completion(const std::string &post_fields);
    std::string post_generate_embedding(const std::string &post_fields);
    std::string post_upload_file(const std::string &filename, const std::string &purpose);
    std::string delete_file(const std::string &file_id);
    std::string post_create_fine_tuning_job(const std::string &post_fields);
    std::string delete_model(const std::string &model_id);

    CURL *handle = NULL;

private:
    void set_project_id();

    struct curl_slist *headers = NULL;
};

Curl::Curl()
{
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    this->handle = curl_easy_init();

    if (this->handle == NULL) {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    const std::optional<std::string> api_key = get_api_key();

    if (not api_key.has_value()) {
        throw std::runtime_error("OPENAI_API_KEY environment variable not set");
    }

    const std::string header = "Authorization: Bearer " + api_key.value();
    this->headers = curl_slist_append(this->headers, header.c_str());

    this->set_project_id();

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

void Curl::set_project_id()
{
    // XXX do we really need this?
    if (not configs.project_id.has_value()) {
        return;
    }

    const std::string header = "OpenAI-Project: " + configs.project_id.value();
    this->headers = curl_slist_append(this->headers, header.c_str());
}

std::string Curl::get_models()
{
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_MODELS.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::get_files()
{
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_FILES.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::post_chat_completion(const std::string &post_fields)
{
    const std::string header = "Content-Type: application/json";
    this->headers = curl_slist_append(this->headers, header.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_CHAT_COMPLETIONS.c_str());
    curl_easy_setopt(this->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::post_generate_embedding(const std::string &post_fields)
{
    const std::string header = "Content-Type: application/json";
    this->headers = curl_slist_append(this->headers, header.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_EMBEDDINGS.c_str());
    curl_easy_setopt(this->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::post_upload_file(const std::string &filename, const std::string &purpose)
{
    const std::string header = "Content-Type: multipart/form-data";
    this->headers = curl_slist_append(this->headers, header.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_FILES.c_str());

    curl_mime *form = curl_mime_init(this->handle);
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

std::string Curl::delete_file(const std::string &file_id)
{
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}", endpoints::URL_FILES, file_id);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::post_create_fine_tuning_job(const std::string &post_fields)
{
    const std::string header = "Content-Type: application/json";
    this->headers = curl_slist_append(this->headers, header.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}", endpoints::URL_FINE_TUNING, "jobs");

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::delete_model(const std::string &model_id)
{
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}", endpoints::URL_MODELS, model_id);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

} // namespace

namespace api {

std::string query_chat_completion_api(const std::string &model, const std::string &prompt, float temperature)
{
    const nlohmann::json messages = { { "role", "user" }, { "content", prompt } };
    const nlohmann::json data = {
        { "model", model }, { "temperature", temperature }, { "messages", nlohmann::json::array({ messages }) }
    };

    Curl curl;
    return curl.post_chat_completion(data.dump());
}

std::string query_embeddings_api(const std::string &model, const std::string &input)
{
    const nlohmann::json data = { { "model", model }, { "input", input } };

    Curl curl;
    return curl.post_generate_embedding(data.dump());
}

std::string query_list_files_api()
{
    Curl curl;
    return curl.get_files();
}

std::string query_models_api()
{
    Curl curl;
    return curl.get_models();
}

std::string query_upload_file_api(const std::string &filename, const std::string &purpose)
{
    Curl curl;
    return curl.post_upload_file(filename, purpose);
}

std::string query_delete_file_api(const std::string &file_id)
{
    Curl curl;
    return curl.delete_file(file_id);
}

std::string query_create_fine_tuning_job_api(const std::string &training_file, const std::string &model)
{
    const nlohmann::json data = { { "model", model }, { "training_file", training_file } };

    Curl curl;
    return curl.post_create_fine_tuning_job(data.dump());
}

std::string query_delete_model(const std::string &model_id)
{
    Curl curl;
    return curl.delete_model(model_id);
}

} // namespace api
