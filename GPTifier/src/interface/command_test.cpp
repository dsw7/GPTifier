#include "interface/command_test.hpp"

#include "interface/model_selector.hpp"
#include "networking/api_openai_user.hpp"
#include "serialization/response_to_json.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

void test_catch_memory_leak()
{
    int *val = new int(5);
    fmt::print("{}\n", *val);
}

void test_create_chat_completion_api()
{
    const nlohmann::json messages = {
        { "role", "user" },
        { "content", "What is 5 + 5? Return the result as follows: >>>{result}<<<" }
    };
    const nlohmann::json data = {
        { "model", select_chat_model() },
        { "temperature", 0.00 },
        { "messages", nlohmann::json::array({ messages }) }
    };

    OpenAIUser api;

    const std::string dump = data.dump();
    const std::string response_1 = api.create_chat_completion(dump);
    const std::string response_2 = api.create_chat_completion(dump);
    const std::string response_3 = api.create_chat_completion(dump);

    const nlohmann::json json_1 = response_to_json(response_1);
    const nlohmann::json json_2 = response_to_json(response_2);
    const nlohmann::json json_3 = response_to_json(response_3);

    const nlohmann::json results = {
        { "result_1", json_1["choices"][0]["message"]["content"] },
        { "result_2", json_2["choices"][0]["message"]["content"] },
        { "result_3", json_3["choices"][0]["message"]["content"] },
    };

    fmt::print("{}\n", results.dump(4));
}

} // namespace

void command_test(int argc, char **argv)
{
    if (argc < 3) {
        throw std::runtime_error("Usage: gpt test (<target>)");
    }

    const std::string target = argv[2];

    if (target == "leak") {
        test_catch_memory_leak();
    } else if (target == "ccc") {
        test_create_chat_completion_api();
    } else {
        throw std::runtime_error("Unknown test target: " + target);
    }
}
