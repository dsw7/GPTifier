#include "cli.hpp"
#include "configs.hpp"
#include "prompts.hpp"
#include "query.hpp"
#include "requests.hpp"
#include "responses.hpp"
#include "typedefs.hpp"
#include "utils.hpp"

#include <iostream>
#include <stdexcept>

void create_chat_completion(const Params &params, const Configs &configs)
{
    ::str_request request = requests::build_chat_request(params.prompt, params.model, params.temperature);

    if (request.empty())
    {
        throw std::runtime_error("Request is empty!");
    }

    requests::print_request(request);

    QueryHandler q;
    ::str_response response = q.run_query(configs.api_key, request);

    if (response.empty())
    {
        throw std::runtime_error("Response is empty!");
    }

    if (params.dump.empty())
    {
        responses::print_chat_completion_response(response);
        responses::export_chat_completion_response(response);
    }
    else
    {
        responses::dump_chat_completion_response(response, params.dump);
    }
}

int main(int argc, char **argv)
{
    Params params;
    if (argc > 1)
    {
        cli::parse_cli(argc, argv, params);
    }

    Configs configs;
    try
    {
        ::read_configs(configs);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        prompt::get_prompt(params);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (params.prompt.empty())
    {
        std::cerr << "Prompt cannot be empty" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        ::create_chat_completion(params, configs);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << std::endl;
    return EXIT_SUCCESS;
}
