#include "command_short.hpp"

#include "api_openai_user.hpp"
#include "cli.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "selectors.hpp"
#include "validation.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

void command_short(int argc, char **argv)
{
    ParamsShort params = cli::get_opts_short(argc, argv);

    float temperature = 1.00;
    if (std::holds_alternative<float>(params.temperature)) {
        temperature = std::get<float>(params.temperature);
    }

    const json messages = { { "role", "user" }, { "content", params.prompt.value() } };
    json data = {
        { "model", select_chat_model() },
        { "temperature", temperature },
        { "messages", json::array({ messages }) },
        { "store", params.store_completion }
    };

    if (params.store_completion) {
        data["metadata"] = { { "prompt", params.prompt.value() } };
    }

    OpenAIUser api;
    const std::string response = api.create_chat_completion(data.dump());
    const json results = parse_response(response);

    if (params.print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    if (not validation::is_chat_completion(results)) {
        throw std::runtime_error("Response from OpenAI is not a chat completion");
    }

    const std::string completion = results["choices"][0]["message"]["content"];

    fmt::print("{}\n", completion);
}
