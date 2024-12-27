#include "command_short.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "json.hpp"
#include "parsers.hpp"
#include "reporting.hpp"
#include "testing.hpp"

#include <fmt/core.h>
#include <string>

namespace {

std::string select_chat_model()
{
    if (testing::is_test_running()) {
        static std::string low_cost_model = "gpt-3.5-turbo";
        fmt::print("Defaulting to using a low cost model: {}\n", low_cost_model);

        return low_cost_model;
    }

    if (configs.chat.model.has_value()) {
        return configs.chat.model.value();
    }

    throw std::runtime_error("No model provided via configuration file!");
}

} // namespace

void command_short(int argc, char **argv)
{
    cli::ParamsShort params = cli::get_opts_short(argc, argv);

    if (not params.prompt.has_value()) {
        throw std::runtime_error("Prompt is empty");
    }

    const std::string model = select_chat_model();
    const std::string response = api::create_chat_completion(model, params.prompt.value(), 1.00);

    if (params.print_raw_json) {
        print_raw_response(response);
        return;
    }

    const nlohmann::json results = parse_response(response);
    const std::string content = results["choices"][0]["message"]["content"];

    fmt::print("{}\n", content);
}
