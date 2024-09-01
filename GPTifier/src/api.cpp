#include "api.hpp"
#include "configs.hpp"

#include <cstdlib>
#include <curl/curl.h>
#include <stdexcept>
#include <string>

namespace
{

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

const std::string get_api_key()
{
    const char *api_key = std::getenv("OPENAI_API_KEY");

    if (api_key)
    {
        return std::string(api_key);
    }

    return std::string();
}

class Curl
{
public:
    Curl();
    ~Curl();

    CURL *handle = NULL;

private:
    struct curl_slist *headers = NULL;
};

Curl::Curl()
{
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
    {
        throw std::runtime_error("Something went wrong when initializing libcurl");
    }

    std::string api_key = get_api_key();

    if (api_key.empty())
    {
        throw std::runtime_error("OPENAI_API_KEY environment variable not set");
    }

    this->handle = curl_easy_init();

    if (this->handle == NULL)
    {
        throw std::runtime_error("Something went wrong when starting libcurl easy session");
    }

    std::string header_auth = "Authorization: Bearer " + api_key;
    this->headers = curl_slist_append(this->headers, header_auth.c_str());

    if (not configs.project_id.empty())
    {
        std::string header_project_id = "OpenAI-Project: " + configs.project_id;
        this->headers = curl_slist_append(this->headers, header_project_id.c_str());
    }

    this->headers = curl_slist_append(this->headers, "Content-Type: application/json");
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    curl_easy_setopt(this->handle, CURLOPT_WRITEFUNCTION, write_callback);
}

Curl::~Curl()
{
    if (this->handle)
    {
        curl_slist_free_all(this->headers);
        curl_easy_cleanup(this->handle);
    }

    curl_global_cleanup();
}

} // namespace

void query_models_api(std::string &response)
{
    static std::string url_get_models = "https://api.openai.com/v1/models";

    Curl curl;

    curl_easy_setopt(curl.handle, CURLOPT_URL, url_get_models.c_str());
    curl_easy_setopt(curl.handle, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl.handle, CURLOPT_WRITEDATA, &response);

    CURLcode rv = curl_easy_perform(curl.handle);

    if (rv != CURLE_OK)
    {
        std::string errmsg = "Failed to run query. " + std::string(curl_easy_strerror(rv));
        throw std::runtime_error(errmsg);
    }
}

void query_embeddings_api(const std::string &post_fields, std::string &response)
{
    static std::string url_embeddings = "https://api.openai.com/v1/embeddings";

    Curl curl;

    curl_easy_setopt(curl.handle, CURLOPT_URL, url_embeddings.c_str());
    curl_easy_setopt(curl.handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl.handle, CURLOPT_POSTFIELDS, post_fields.c_str());
    curl_easy_setopt(curl.handle, CURLOPT_WRITEDATA, &response);

    CURLcode rv = curl_easy_perform(curl.handle);

    if (rv != CURLE_OK)
    {
        std::string errmsg = "Failed to run query. " + std::string(curl_easy_strerror(rv));
        throw std::runtime_error(errmsg);
    }
}

void query_chat_completion_api(const std::string &post_fields, std::string &response)
{
    static std::string url_chat_completions = "https://api.openai.com/v1/chat/completions";

    Curl curl;

    curl_easy_setopt(curl.handle, CURLOPT_URL, url_chat_completions.c_str());
    curl_easy_setopt(curl.handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl.handle, CURLOPT_POSTFIELDS, post_fields.c_str());
    curl_easy_setopt(curl.handle, CURLOPT_WRITEDATA, &response);

    CURLcode rv = curl_easy_perform(curl.handle);

    if (rv != CURLE_OK)
    {
        std::string errmsg = "Failed to run query. " + std::string(curl_easy_strerror(rv));
        throw std::runtime_error(errmsg);
    }
}
