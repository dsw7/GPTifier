#include "command_short.hpp"

#include "cli.hpp"
#include "configs.hpp"
#include "reporting.hpp"
#include "testing.hpp"
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <string>

namespace
{

std::string select_chat_model()
{
    if (testing::is_test_running())
    {
        static std::string low_cost_model = "gpt-3.5-turbo";
        std::cout << "Defaulting to using a low cost model: " << low_cost_model << '\n';

        return low_cost_model;
    }

    if (configs.chat.model.empty())
    {
        throw std::runtime_error("No model provided via configuration file!");
    }

    return configs.chat.model;
}

std::string get_post_fields(const std::string &model, const std::string &prompt)
{
    nlohmann::json body = {};
    body["model"] = model;
    body["temperature"] = 1.00;

    nlohmann::json messages = {};
    messages["role"] = "user";
    messages["content"] = prompt;

    body["messages"] = nlohmann::json::array({messages});

    std::string post_fields = body.dump(2);
    return post_fields;
}

} // namespace

void command_short(const int argc, char **argv)
{
    std::string prompt = cli::get_opts_short(argc, argv);
    std::string model = select_chat_model();
    std::cout << get_post_fields(model, prompt);
}
