#include "command_test.hpp"

#include "networking/api_openai_user.hpp"
#include "parsers.hpp"
#include "selectors.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

using json = nlohmann::json;

namespace {

void test_catch_memory_leak(bool free_memory)
{
    int *val = new int(5);

    fmt::print("{}\n", *val);

    if (free_memory) {
        delete val;
    }
}

void test_create_chat_completion_api()
{
    const json messages = {
        { "role", "user" },
        { "content", "What is 5 + 5? Return the result as follows: >>>{result}<<<" }
    };
    const json data = {
        { "model", select_chat_model() },
        { "temperature", 0.00 },
        { "messages", json::array({ messages }) }
    };

    OpenAIUser api;

    const std::string dump = data.dump();
    const std::string c_1 = api.create_chat_completion(dump);
    const std::string c_2 = api.create_chat_completion(dump);
    const std::string c_3 = api.create_chat_completion(dump);

    const json r_1 = parse_response(c_1);
    const json r_2 = parse_response(c_2);
    const json r_3 = parse_response(c_3);

    const json results = {
        { "result_1", r_1["choices"][0]["message"]["content"] },
        { "result_2", r_2["choices"][0]["message"]["content"] },
        { "result_3", r_3["choices"][0]["message"]["content"] },
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

    if (target == "mem+") {
        test_catch_memory_leak(false);
    } else if (target == "mem-") {
        test_catch_memory_leak(true);
    } else if (target == "ccc") {
        test_create_chat_completion_api();
    } else {
        throw std::runtime_error("Unknown test target: " + target);
    }
}
