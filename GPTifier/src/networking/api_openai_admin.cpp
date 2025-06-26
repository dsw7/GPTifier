#include "api_openai_admin.hpp"

#include "curl_.hpp"

#include <cstdlib>
#include <fmt/core.h>
#include <stdexcept>

namespace {

const std::string URL_ORGANIZATION = "https://api.openai.com/v1/organization";

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

namespace networking {

std::string get_costs(const std::time_t &start_time, int limit)
{
    Curl curl;
    CURL *handle = curl.get_handle();

    curl_slist *headers = curl.get_headers();
    headers = curl_slist_append(headers, ("Authorization: Bearer " + get_admin_api_key()).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

    const std::string endpoint = fmt::format("{}/{}?start_time={}&limit={}", URL_ORGANIZATION, "costs", start_time, limit);
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

} // namespace networking
