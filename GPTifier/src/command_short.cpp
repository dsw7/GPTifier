#include "command_short.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "selectors.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

void command_short(int argc, char **argv)
{
    ParamsShort params = cli::get_opts_short(argc, argv);

    if (not params.prompt.has_value()) {
        throw std::runtime_error("Prompt is empty");
    }

    const json messages = { { "role", "user" }, { "content", params.prompt.value() } };
    const json data = {
        { "model", select_chat_model() }, { "temperature", 1.00 }, { "messages", json::array({ messages }) }
    };

    Curl curl;
    const std::string response = curl.create_chat_completion(data.dump());
    const json results = parse_response(response);

    if (params.print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    const std::string content = results["choices"][0]["message"]["content"];

    fmt::print("{}\n", content);
}
