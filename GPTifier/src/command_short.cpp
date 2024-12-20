#include "command_short.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "json.hpp"
#include "reporting.hpp"
#include "testing.hpp"

#include <fmt/core.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

namespace {

std::string select_chat_model()
{
    if (testing::is_test_running()) {
        static std::string low_cost_model = "gpt-3.5-turbo";
        std::cout << "Defaulting to using a low cost model: " << low_cost_model << '\n';

        return low_cost_model;
    }

    if (configs.chat.model.has_value()) {
        return configs.chat.model.value();
    }

    throw std::runtime_error("No model provided via configuration file!");
}

void print_chat_completion_response(const std::string &response)
{
    const nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error")) {
        const std::string error = results["error"]["message"];
        std::cout << error;
    } else {
        const std::string content = results["choices"][0]["message"]["content"];
        std::cout << content;
    }

    std::cout << '\n';
}

} // namespace

void command_short(int argc, char **argv)
{
    std::optional<std::string> prompt = cli::get_opts_short(argc, argv);

    if (not prompt.has_value()) {
        throw std::runtime_error("Prompt is empty");
    }

    const std::string model = select_chat_model();

    std::string response;
    try {
        response = query_chat_completion_api(model, prompt.value(), 1.00);
    } catch (std::runtime_error &e) {
        const std::string errmsg = fmt::format("Query failed. {}", e.what());
        throw std::runtime_error(errmsg);
    }

    print_chat_completion_response(response);
}
