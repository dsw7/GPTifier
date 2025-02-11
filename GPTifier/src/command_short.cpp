#include "command_short.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "params.hpp"
#include "parsers.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

namespace {

std::string select_chat_model()
{
#ifdef TESTING_ENABLED
    static std::string low_cost_model = "gpt-3.5-turbo";
    return low_cost_model;
#endif

    if (configs.chat.model.has_value()) {
        return configs.chat.model.value();
    }

    throw std::runtime_error("No model provided via configuration file!");
}

} // namespace

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

    if (params.print_raw_json) {
        print_raw_response(response);
        return;
    }

    const json results = parse_response(response);
    const std::string content = results["choices"][0]["message"]["content"];

    fmt::print("{}\n", content);
}
