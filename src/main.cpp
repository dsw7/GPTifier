#include "query.h"
#include "presentation.h"

#include <filesystem>
#include <fstream>
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

void load_api_key(std::string &api_key)
{
    const char* home_dir = std::getenv("HOME");

    if (not home_dir)
    {
        throw std::runtime_error("Could not locate home directory!");
    }

    std::string path_key = std::string(home_dir) + "/.openai";

    if (not std::filesystem::exists(path_key))
    {
        throw std::runtime_error("Could not locate .openai key file in home directory!");
    }

    std::ifstream s_key(path_key);
    std::getline(s_key, api_key);

    s_key.close();
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
    std::string prompt;
    ::read_prompt(prompt);

    if (prompt.empty())
    {
        std::cerr << "Prompt cannot be empty" << std::endl;
        return EXIT_FAILURE;
    }

    std::string api_key;

    try
    {
        ::load_api_key(api_key);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::string payload;
    ::build_payload(prompt, payload);

    presentation::print_payload(payload);
    std::string reply;

    try
    {
        QueryHandler q(api_key);
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
