#include "api_openai_user.hpp"

#include "configs.hpp"

#include <cstdlib>
#include <fmt/core.h>
#include <stdexcept>

namespace {

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

namespace endpoints {

const std::string URL_CHAT_COMPLETIONS = "https://api.openai.com/v1/chat/completions";
const std::string URL_EMBEDDINGS = "https://api.openai.com/v1/embeddings";
const std::string URL_FILES = "https://api.openai.com/v1/files";
const std::string URL_FINE_TUNING = "https://api.openai.com/v1/fine_tuning";
const std::string URL_MODELS = "https://api.openai.com/v1/models";

} // namespace endpoints

namespace networking {

void OpenAIUser::reset_handle()
{
    this->reset_easy_handle();
    this->reset_headers_list();

    this->set_writefunction();
    this->set_auth_token(get_user_api_key());
}

std::string OpenAIUser::get_models()
{
    this->reset_handle();

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_MODELS.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIUser::get_uploaded_files(bool sort_asc)
{
    this->reset_handle();

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string order = sort_asc ? "asc" : "desc";
    const std::string endpoint = fmt::format("{}?order={}", endpoints::URL_FILES, order);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIUser::create_chat_completion(const std::string &post_fields)
{
    this->reset_handle();

    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_CHAT_COMPLETIONS.c_str());

    curl_easy_setopt(this->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIUser::get_chat_completions(int limit)
{
    this->reset_handle();

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}?limit={}", endpoints::URL_CHAT_COMPLETIONS, limit);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIUser::delete_chat_completion(const std::string &chat_completion_id)
{
    this->reset_handle();

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}", endpoints::URL_CHAT_COMPLETIONS, chat_completion_id);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIUser::create_embedding(const std::string &post_fields)
{
    this->reset_handle();

    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoints::URL_EMBEDDINGS.c_str());

    curl_easy_setopt(this->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIUser::upload_file(const std::string &filename, const std::string &purpose)
{
    this->reset_handle();

    this->set_content_type_submit_form();
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

    const CURLcode code = curl_easy_perform(this->handle);
    curl_mime_free(form);

    if (code != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(code));
    }

    return response;
}

std::string OpenAIUser::delete_file(const std::string &file_id)
{
    this->reset_handle();

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}", endpoints::URL_FILES, file_id);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIUser::create_fine_tuning_job(const std::string &post_fields)
{
    this->reset_handle();

    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}", endpoints::URL_FINE_TUNING, "jobs");
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIUser::delete_model(const std::string &model_id)
{
    this->reset_handle();

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}", endpoints::URL_MODELS, model_id);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIUser::get_fine_tuning_jobs(const std::string &limit)
{
    this->reset_handle();

    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}?limit={}", endpoints::URL_FINE_TUNING, "jobs", limit);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

} // namespace networking
