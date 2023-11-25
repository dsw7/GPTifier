#include "cli.h"
#include "configs.h"
#include "prompts.h"
#include "query.h"
#include "utils.h"

#include <iostream>
#include <stdexcept>

void run_basic_query(const Params &params, const Configs &configs)
{
    QueryHandler q;

    q.build_payload(params.prompt, configs.model, params.temperature);
    q.print_payload();
    q.run_query(configs.api_key);
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
        ::run_basic_query(params, configs);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << std::endl;
    return EXIT_SUCCESS;
}
