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

Configs configs;

void create_chat_completion(const Params &params)
{
    ::str_request request = requests::build_chat_request(params.prompt, params.model, params.temperature);

    if (request.empty())
    {
        throw std::runtime_error("Request is empty!");
    }

    requests::print_request(request);

    QueryHandler q;
    ::str_response response = q.run_query(request);

    if (response.empty())
    {
        throw std::runtime_error("Response is empty!");
    }

    if (params.dump.empty())
    {
        responses::print_chat_completion_response(response);
        if (params.enable_export)
        {
            responses::export_chat_completion_response(response);
        }
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

    try
    {
        ::read_configs();
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
        ::create_chat_completion(params);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << std::endl;
    return EXIT_SUCCESS;
}
