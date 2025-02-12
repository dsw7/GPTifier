#include "api.hpp"

#include "configs.hpp"

#include <cstdlib>
#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace endpoints {

const std::string URL_CHAT_COMPLETIONS = "https://api.openai.com/v1/chat/completions";
const std::string URL_EMBEDDINGS = "https://api.openai.com/v1/embeddings";
const std::string URL_FILES = "https://api.openai.com/v1/files";
const std::string URL_FINE_TUNING = "https://api.openai.com/v1/fine_tuning";
const std::string URL_MODELS = "https://api.openai.com/v1/models";
const std::string URL_ORGANIZATION = "https://api.openai.com/v1/organization";

} // namespace endpoints

namespace {

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

void catch_curl_error(CURLcode code)
{
    if (code != CURLE_OK) {
        const std::string errmsg = "Failed to run query. " + std::string(curl_easy_strerror(code));
        throw std::runtime_error(errmsg);
    }
}

} // namespace

Curl::Curl()
{
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    this->handle = curl_easy_init();

    if (this->handle == NULL) {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

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

void Curl::set_admin_key()
{
    const char *admin_key = std::getenv("OPENAI_ADMIN_KEY");

    if (admin_key == NULL) {
        throw std::runtime_error("OPENAI_ADMIN_KEY environment variable not set");
    }

    const std::string header = fmt::format("Authorization: Bearer {}", admin_key);
    this->headers = curl_slist_append(this->headers, header.c_str());
}

void Curl::set_api_key()
{
    const char *api_key = std::getenv("OPENAI_API_KEY");

    if (api_key == NULL) {
        throw std::runtime_error("OPENAI_API_KEY environment variable not set");
    }

    const std::string header = fmt::format("Authorization: Bearer {}", api_key);
    this->headers = curl_slist_append(this->headers, header.c_str());
}

void Curl::set_project_id()
{
    if (not configs.project_id.has_value()) {
        return;
    }

    const std::string header = "OpenAI-Project: " + configs.project_id.value();
    this->headers = curl_slist_append(this->headers, header.c_str());
}

void Curl::set_content_type_transmit_json()
{
    const std::string header = "Content-Type: application/json";
    this->headers = curl_slist_append(this->headers, header.c_str());
}

// Admin commands
std::string Curl::get_costs(const std::time_t &start_time)
{
    this->set_admin_key();
    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}?start_time={}", endpoints::URL_ORGANIZATION, "costs", start_time);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

// User commands
std::string Curl::get_models()
{
    this->set_api_key();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_MODELS.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::get_uploaded_files()
{
    this->set_api_key();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_FILES.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::create_chat_completion(const std::string &post_fields)
{
    this->set_api_key();
    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_CHAT_COMPLETIONS.c_str());
    curl_easy_setopt(this->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::create_embedding(const std::string &post_fields)
{
    this->set_api_key();
    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_EMBEDDINGS.c_str());
    curl_easy_setopt(this->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::upload_file(const std::string &filename, const std::string &purpose)
{
    this->set_api_key();

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
    this->set_api_key();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}", endpoints::URL_FILES, file_id);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::create_fine_tuning_job(const std::string &post_fields)
{
    this->set_api_key();
    this->set_content_type_transmit_json();
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
    this->set_api_key();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}", endpoints::URL_MODELS, model_id);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string Curl::get_fine_tuning_jobs(const std::string &limit)
{
    this->set_api_key();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}?limit={}", endpoints::URL_FINE_TUNING, "jobs", limit);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}
