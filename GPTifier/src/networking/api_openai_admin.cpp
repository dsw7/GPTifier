#include "api_openai_admin.hpp"

#include "configs.hpp"

#include <cstdlib>
#include <fmt/core.h>
#include <stdexcept>

namespace {

std::string get_admin_api_key()
{
    static std::string api_key;

    if (api_key.empty()) {
        const char *env_api_key = std::getenv("OPENAI_ADMIN_KEY");

        if (env_api_key == nullptr) {
            throw std::runtime_error("OPENAI_ADMIN_KEY environment variable not set");
        }

        api_key = env_api_key;
    }
    return api_key;
}

} // namespace

namespace endpoints {

const std::string URL_ORGANIZATION = "https://api.openai.com/v1/organization";

} // namespace endpoints

namespace networking {

void OpenAIAdmin::reset_handle()
{
    this->reset_easy_handle();
    this->reset_headers_list();

    this->set_writefunction();
    this->set_auth_token(get_admin_api_key());
}

std::string OpenAIAdmin::get_costs(const std::time_t &start_time, int limit)
{
    this->reset_handle();

    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format(
        "{}/{}?start_time={}&limit={}", endpoints::URL_ORGANIZATION, "costs", start_time, limit);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

std::string OpenAIAdmin::get_users(int limit)
{
    this->reset_handle();

    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}?limit={}", endpoints::URL_ORGANIZATION, "users", limit);
    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());

    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    this->run_easy_perform();
    return response;
}

} // namespace networking
