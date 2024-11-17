#include "command_short.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "configs.hpp"
#include "json.hpp"
#include "reporting.hpp"
#include "request_bodies.hpp"
#include "testing.hpp"

#include <fmt/core.h>
#include <iostream>
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

    if (configs.chat.model.empty()) {
        throw std::runtime_error("No model provided via configuration file!");
    }

    return configs.chat.model;
}

void print_chat_completion_response(const std::string &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error")) {
        std::string error = results["error"]["message"];
        std::cout << error;
    } else {
        std::string content = results["choices"][0]["message"]["content"];
        std::cout << content;
    }

    std::cout << '\n';
}

} // namespace

void command_short(const int argc, char **argv)
{
    std::string prompt = cli::get_opts_short(argc, argv);
    std::string model = select_chat_model();
    std::string request_body = get_chat_completion_request_body(model, prompt, 1.00);

    std::string response;
    try {
        response = query_chat_completion_api(request_body);
    } catch (std::runtime_error &e) {
        std::string errmsg = fmt::format("Query failed. {}", e.what());
        throw std::runtime_error(errmsg);
    }

    print_chat_completion_response(response);
}
