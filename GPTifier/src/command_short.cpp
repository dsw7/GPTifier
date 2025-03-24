#include "command_short.hpp"

#include "cli.hpp"
#include "networking/api_openai_user.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "selectors.hpp"
#include "validation.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

namespace {

void create_chat_completion(json &results, const std::string &prompt, float temperature, bool store_completion)
{
    const json messages = { { "role", "user" }, { "content", prompt } };
    json data = {
        { "model", select_chat_model() },
        { "temperature", temperature },
        { "messages", json::array({ messages }) },
        { "store", store_completion }
    };

    if (store_completion) {
        data["metadata"] = { { "prompt", prompt } };
    }

    OpenAIUser api;

    const std::string response = api.create_chat_completion(data.dump());
    results = parse_response(response);
}

} // namespace

void command_short(int argc, char **argv)
{
    ParamsShort params = cli::get_opts_short(argc, argv);

    float temperature = 1.00;
    if (std::holds_alternative<float>(params.temperature)) {
        temperature = std::get<float>(params.temperature);
    }

    json results;
    create_chat_completion(results, params.prompt.value(), temperature, params.store_completion);

    if (params.print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    validation::is_chat_completion(results);

    const std::string completion = results["choices"][0]["message"]["content"];
    fmt::print("{}\n", completion);
}
