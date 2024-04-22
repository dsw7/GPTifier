#include "requests.hpp"
#include "params.hpp"
#include "utils.hpp"

#include <iostream>
#include <json.hpp>
#include <stdexcept>

namespace requests
{

std::string build_chat_request()
{
    nlohmann::json body = {};

    body["model"] = ::params.model;

    try
    {
        body["temperature"] = std::stof(::params.temperature);
    }
    catch (std::invalid_argument &e)
    {
        std::string errmsg = std::string(e.what()) + ". Failed to convert '" + ::params.temperature + "' to float";
        throw std::runtime_error(errmsg);
    }

    nlohmann::json messages = {};
    messages["role"] = "user";
    messages["content"] = ::params.prompt;

    body["messages"] = nlohmann::json::array({messages});

    return body.dump(2);
}

void print_request(const std::string &request)
{
    ::print_separator();
    std::cout << "\033[1mRequest:\033[0m " + request + '\n';
    ::print_separator();
}

} // namespace requests
