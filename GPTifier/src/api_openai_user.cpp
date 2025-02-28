#include "api_openai_user.hpp"

#include "configs.hpp"

#include <cstdlib>
#include <fmt/core.h>
#include <stdexcept>

namespace endpoints {

const std::string URL_CHAT_COMPLETIONS = "https://api.openai.com/v1/chat/completions";
const std::string URL_EMBEDDINGS = "https://api.openai.com/v1/embeddings";
const std::string URL_FILES = "https://api.openai.com/v1/files";
const std::string URL_FINE_TUNING = "https://api.openai.com/v1/fine_tuning";
const std::string URL_MODELS = "https://api.openai.com/v1/models";

} // namespace endpoints

void OpenAIUser::set_api_key()
{
    const char *api_key = std::getenv("OPENAI_API_KEY");

    if (api_key == NULL) {
        throw std::runtime_error("OPENAI_API_KEY environment variable not set");
    }

    const std::string header = fmt::format("Authorization: Bearer {}", api_key);
    this->headers = curl_slist_append(this->headers, header.c_str());
}

void OpenAIUser::set_project_id()
{
    if (not configs.project_id.has_value()) {
        return;
    }

    const std::string header = "OpenAIUser-Project: " + configs.project_id.value();
    this->headers = curl_slist_append(this->headers, header.c_str());
}

std::string OpenAIUser::get_models()
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

std::string OpenAIUser::get_uploaded_files()
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

std::string OpenAIUser::create_chat_completion(const std::string &post_fields)
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

std::string OpenAIUser::get_chat_completions(const std::string &limit)
{
    this->set_api_key();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}?limit={}", endpoints::URL_CHAT_COMPLETIONS, limit);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string OpenAIUser::create_embedding(const std::string &post_fields)
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

std::string OpenAIUser::upload_file(const std::string &filename, const std::string &purpose)
{
    this->set_api_key();
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

    const CURLcode rv = curl_easy_perform(this->handle);
    curl_mime_free(form);

    catch_curl_error(rv);
    return response;
}

std::string OpenAIUser::delete_file(const std::string &file_id)
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

std::string OpenAIUser::create_fine_tuning_job(const std::string &post_fields)
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

std::string OpenAIUser::delete_model(const std::string &model_id)
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

std::string OpenAIUser::get_fine_tuning_jobs(const std::string &limit)
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
