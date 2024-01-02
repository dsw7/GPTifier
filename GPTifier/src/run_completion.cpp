#include "run_completion.hpp"

#include "params.hpp"
#include "prompts.hpp"
#include "query.hpp"
#include "requests.hpp"
#include "responses.hpp"
#include "typedefs.hpp"

#include <stdexcept>

void create_chat_completion()
{
    try
    {
        prompt::get_prompt();
    }
    catch (std::runtime_error &e)
    {
        throw;
    }

    if (::params.prompt.empty())
    {
        throw std::runtime_error("Prompt cannot be empty");
    }

    ::str_request request = requests::build_chat_request();
    requests::print_request(request);

    QueryHandler q;
    ::str_response response = q.run_query(request);

    if (response.empty())
    {
        throw std::runtime_error("Response is empty!");
    }

    if (::params.dump.empty())
    {
        responses::print_chat_completion_response(response);
        if (::params.enable_export)
        {
            responses::export_chat_completion_response(response);
        }
    }
    else
    {
        responses::dump_chat_completion_response(response);
    }
}
