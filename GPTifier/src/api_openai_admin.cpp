#include "api_openai_admin.hpp"

#include "configs.hpp"

#include <cstdlib>
#include <fmt/core.h>
#include <stdexcept>

namespace endpoints {

const std::string URL_ORGANIZATION = "https://api.openai.com/v1/organization";

} // namespace endpoints

void OpenAIAdmin::set_admin_key()
{
    const char *admin_key = std::getenv("OPENAI_ADMIN_KEY");

    if (admin_key == NULL) {
        throw std::runtime_error("OPENAI_ADMIN_KEY environment variable not set");
    }

    const std::string header = fmt::format("Authorization: Bearer {}", admin_key);
    this->headers = curl_slist_append(this->headers, header.c_str());
}

std::string OpenAIAdmin::get_costs(const std::time_t &start_time, int limit)
{
    this->set_admin_key();
    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format(
        "{}/{}?start_time={}&limit={}", endpoints::URL_ORGANIZATION, "costs", start_time, limit);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}

std::string OpenAIAdmin::get_users(int limit)
{
    this->set_admin_key();
    this->set_content_type_transmit_json();
    curl_easy_setopt(this->handle, CURLOPT_HTTPHEADER, this->headers);

    const std::string endpoint = fmt::format("{}/{}?limit={}", endpoints::URL_ORGANIZATION, "users", limit);

    curl_easy_setopt(this->handle, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->handle, CURLOPT_HTTPGET, 1L);

    std::string response;
    curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, &response);

    catch_curl_error(curl_easy_perform(this->handle));
    return response;
}
