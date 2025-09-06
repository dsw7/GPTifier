#include "chat_completions.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"

#include <stdexcept>

namespace serialization {

namespace {

void unpack_chat_completion(const nlohmann::json &json, ChatCompletion &cc)
{
    cc.completion = json["choices"][0]["message"]["content"];
    cc.completion_tokens = json["usage"]["completion_tokens"];
    cc.created = json["created"];
    cc.model = json["model"];
    cc.prompt_tokens = json["usage"]["prompt_tokens"];
}

} // namespace

ChatCompletion create_chat_completion(const std::string &prompt, const std::string &model, float temp, bool store_completion)
{
    const nlohmann::json messages = { { "role", "user" }, { "content", prompt } };
    nlohmann::json data = {
        { "model", model },
        { "temperature", temp },
        { "messages", nlohmann::json::array({ messages }) },
        { "store", store_completion }
    };

    if (store_completion) {
        data["metadata"] = { { "prompt", prompt } };
    }

    const auto start = std::chrono::high_resolution_clock::now();
    const std::string response = networking::create_chat_completion(data.dump());
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> rtt = end - start;

    ChatCompletion cc = unpack_response<ChatCompletion>(response, unpack_chat_completion);
    cc.prompt = prompt;
    cc.raw_response = response;
    cc.rtt = rtt;
    return cc;
}

} // namespace serialization
