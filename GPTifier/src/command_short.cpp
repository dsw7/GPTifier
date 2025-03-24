#include "command_short.hpp"

#include "cli.hpp"
#include "networking/api_openai_user.hpp"
#include "params.hpp"
#include "selectors.hpp"
#include "serialization/chat_completions.hpp"

#include <fmt/core.h>
#include <string>

void command_short(int argc, char **argv)
{
    ParamsShort params = cli::get_opts_short(argc, argv);

    float temperature = 1.00;
    if (std::holds_alternative<float>(params.temperature)) {
        temperature = std::get<float>(params.temperature);
    }

    const std::string model = select_chat_model();
    ChatCompletion cc = create_chat_completion(params.prompt.value(), model, temperature, params.store_completion);

    if (params.print_raw_json) {
        fmt::print("{}\n", cc.raw_response);
        return;
    }

    fmt::print("{}\n", cc.completion);
}
