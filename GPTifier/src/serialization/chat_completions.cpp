#include "chat_completions.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"

#include <fmt/core.h>

namespace serialization {

namespace {

void unpack_chat_completions(const nlohmann::json &json, ChatCompletions &ccs)
{
    for (const auto &entry: json["data"]) {
        ChatCompletion cc;

        if (entry["metadata"].contains("prompt")) {
            cc.prompt = entry["metadata"]["prompt"];
        }

        cc.completion = entry["choices"][0]["message"]["content"];
        cc.created = entry["created"];
        cc.id = entry["id"];

        ccs.completions.push_back(cc);
    }
}

} // namespace

ChatCompletions get_chat_completions(int limit)
{
    const std::string response = networking::get_chat_completions(limit);
    const nlohmann::json json = response_to_json(response);

    ChatCompletions chat_completions;
    chat_completions.raw_response = response;

    try {
        unpack_chat_completions(json, chat_completions);
    } catch (nlohmann::json::out_of_range &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    } catch (nlohmann::json::type_error &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return chat_completions;
}

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

    auto start = std::chrono::high_resolution_clock::now();
    const std::string response = networking::create_chat_completion(data.dump());
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> rtt = end - start;

    const nlohmann::json json = response_to_json(response);
    ChatCompletion cc;

    try {
        cc.completion = json["choices"][0]["message"]["content"];
        cc.completion_tokens = json["usage"]["completion_tokens"];
        cc.created = json["created"];
        cc.model = json["model"];
        cc.prompt = prompt;
        cc.prompt_tokens = json["usage"]["prompt_tokens"];
        cc.raw_response = response;
        cc.rtt = rtt;
    } catch (nlohmann::json::out_of_range &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    } catch (nlohmann::json::type_error &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return cc;
}

bool delete_chat_completion(const std::string &chat_completion_id)
{
    networking::OpenAIUser api;
    const std::string response = api.delete_chat_completion(chat_completion_id);
    const nlohmann::json json = response_to_json(response);

    if (not json.contains("deleted")) {
        throw std::runtime_error("Malformed response. Missing 'deleted' key");
    }

    return json["deleted"];
}

} // namespace serialization
