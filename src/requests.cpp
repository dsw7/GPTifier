#include "requests.h"
#include "utils.h"

#include <iostream>
#include <json.hpp>
#include <stdexcept>

namespace requests
{

::req_str build_chat_request(const std::string &prompt, const std::string &model, const std::string &temp)
{
    // Build "chat completion" request
    // See https://platform.openai.com/docs/api-reference/chat/create

    nlohmann::json body = {};

    body["model"] = model;

    try
    {
        body["temperature"] = std::stof(temp);
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << std::endl;
        throw std::runtime_error("Failed to convert '" + temp + "' to float");
    }

    nlohmann::json messages = {};
    messages["role"] = "user";
    messages["content"] = prompt;

    body["messages"] = nlohmann::json::array({messages});

    return body.dump(2);
}

void print_request(const ::req_str &request)
{
    utils::print_separator();

    if (request.empty())
    {
        throw std::runtime_error("Request is empty!");
    }

    std::cout << "\033[1mRequest:\033[0m " + request + '\n';

    utils::print_separator();
}

} // namespace requests
