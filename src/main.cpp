#include "cli.h"
#include "configs.h"
#include "prompts.h"
#include "query.h"
#include "requests.h"
#include "utils.h"

#include <iostream>
#include <stdexcept>

void create_chat_completion(const Params &params, const Configs &configs)
{
    ::req_str request = requests::build_chat_request(params.prompt, configs.model, params.temperature);
    requests::print_request(request);

    QueryHandler q;
    q.run_query(configs.api_key, request);
    q.print_response();

    if (not params.dump.empty())
    {
        q.dump_response(params.dump);
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
