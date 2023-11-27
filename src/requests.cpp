#include "requests.hpp"
#include "utils.hpp"

#include <iostream>
#include <json.hpp>
#include <stdexcept>

namespace requests
{

::str_request build_chat_request(const std::string &prompt, const std::string &model, const std::string &temp)
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
        std::cerr << e.what() << ". Failed to convert '" + temp + "' to float" << std::endl;
        return std::string();
    }

    nlohmann::json messages = {};
    messages["role"] = "user";
    messages["content"] = prompt;

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
