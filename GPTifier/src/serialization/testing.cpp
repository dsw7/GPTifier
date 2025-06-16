#include "testing.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"

#include <iostream>
#include <json.hpp>

namespace serialization {

void test_catch_memory_leak()
{
    int *val = new int(5);
    std::cout << *val << '\n';
}

std::string test_curl_handle_is_reusable()
{
    static std::string low_cost_model = "gpt-3.5-turbo";
    const nlohmann::json messages = {
        { "role", "user" },
        { "content", "What is 5 + 5? Return the result as follows: >>>{result}<<<" }
    };
    const nlohmann::json data = {
        { "model", low_cost_model },
        { "temperature", 0.00 },
        { "messages", nlohmann::json::array({ messages }) }
    };

    networking::OpenAIUser api;

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

    return results.dump(4);
}

} // namespace serialization
