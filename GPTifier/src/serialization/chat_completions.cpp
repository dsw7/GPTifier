#include "serialization/chat_completions.hpp"

#include "networking/api_openai_user.hpp"
#include "serialization/parse_response.hpp"
#include "serialization/response_to_json.hpp"
#include "serialization/validation.hpp"

#include <fmt/core.h>

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
    OpenAIUser api;
    const std::string response = api.get_chat_completions(limit);
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

    OpenAIUser api;

    auto start = std::chrono::high_resolution_clock::now();
    const std::string response = api.create_chat_completion(data.dump());
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> rtt = end - start;

    const nlohmann::json results = parse_response(response);
    validation::is_chat_completion(results);

    ChatCompletion cc;

    cc.completion = results["choices"][0]["message"]["content"];
    cc.completion_tokens = results["usage"]["completion_tokens"];
    cc.created = results["created"];
    cc.model = results["model"];
    cc.prompt = prompt;
    cc.prompt_tokens = results["usage"]["prompt_tokens"];
    cc.raw_response = response;
    cc.rtt = rtt;

    return cc;
}

bool delete_chat_completion(const std::string &chat_completion_id)
{
    OpenAIUser api;
    const std::string response = api.delete_chat_completion(chat_completion_id);
    const nlohmann::json json = response_to_json(response);

    if (not json.contains("deleted")) {
        throw std::runtime_error("Malformed response. Missing 'deleted' key");
    }

    return json["deleted"];
}
