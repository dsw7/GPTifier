#include "run_completion.hpp"

#include "params.hpp"
#include "prompts.hpp"
#include "query.hpp"
#include "requests.hpp"
#include "responses.hpp"
#include "typedefs.hpp"

void create_chat_completion()
{
    prompt::get_prompt();

    ::str_request request = requests::build_chat_request();
    requests::print_request(request);

    QueryHandler q;
    ::str_response response = q.run_query(request);

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
