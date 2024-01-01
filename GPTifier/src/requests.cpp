#include "requests.hpp"
#include "data.hpp"
#include "utils.hpp"

#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>

namespace requests
{

::str_request build_chat_request()
{
    nlohmann::json body = {};

    body["model"] = params.model;

    try
    {
        body["temperature"] = std::stof(params.temperature);
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << ". Failed to convert '" + params.temperature + "' to float" << std::endl;
        return std::string();
    }

    nlohmann::json messages = {};
    messages["role"] = "user";
    messages["content"] = params.prompt;

    body["messages"] = nlohmann::json::array({messages});

    return body.dump(2);
}

void print_request(const ::str_request &request)
{
    utils::print_separator();
    std::cout << "\033[1mRequest:\033[0m " + request + '\n';
    utils::print_separator();
}

} // namespace requests
