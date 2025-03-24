#include "serialization/chat_completions.hpp"

#include "networking/api_openai_user.hpp"
#include "parsers.hpp"
#include "validation.hpp"

using json = nlohmann::json;

json jsonify_cc(const ChatCompletion &cc)
{
    json results;

    results["completion"] = cc.completion;
    results["completion_tokens"] = cc.completion_tokens;
    results["created"] = cc.created;
    results["id"] = cc.id;
    results["model"] = cc.model;
    results["prompt"] = cc.prompt;
    results["prompt_tokens"] = cc.prompt_tokens;
    results["rtt"] = cc.rtt.count();

    return results;
}

ChatCompletion create_chat_completion(
    const std::string &prompt, const std::string &model,
    float temperature, bool store_completion)
{
    const json messages = { { "role", "user" }, { "content", prompt } };
    json data = {
        { "model", model },
        { "temperature", temperature },
        { "messages", json::array({ messages }) },
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

    const json results = parse_response(response);
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
