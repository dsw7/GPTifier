#include "configs.h"
#include "presentation.h"
#include "query.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

void read_prompt(std::string &prompt)
{
    presentation::print_separator();
    std::cout << "\033[1mInput:\033[0m ";
    std::getline(std::cin, prompt);
}

void build_payload(const std::string &prompt, std::string &payload)
{
    nlohmann::json messages = {};

    messages["role"] = "user";
    messages["content"] = prompt;

    nlohmann::json body = {};
    body["messages"] = {messages};
    body["model"] = "gpt-3.5-turbo";

    payload = body.dump(2);
}

int main()
{
    Configs configs;

    try
    {
        ::read_configs(configs);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::string prompt;
    ::read_prompt(prompt);

    if (prompt.empty())
    {
        std::cerr << "Prompt cannot be empty" << std::endl;
        return EXIT_FAILURE;
    }

    std::string payload;
    ::build_payload(prompt, payload);

    presentation::print_payload(payload);
    std::string reply;

    try
    {
        QueryHandler q(configs.api_key);
        q.run_query(payload, reply);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    presentation::print_results(reply);
    return EXIT_SUCCESS;
}
