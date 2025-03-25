#include "serialization/chat_completions.hpp"

#include "networking/api_openai_user.hpp"
#include "serialization/parse_response.hpp"
#include "validation.hpp"

namespace {

void unpack_chat_completions(const nlohmann::json &results, ChatCompletions &ccs)
{
    for (const auto &entry: results["data"]) {
        validation::is_chat_completion(entry);

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

nlohmann::json jsonify_cc(const ChatCompletion &cc)
{
    nlohmann::json results;

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

ChatCompletions get_chat_completions(int limit)
{
    OpenAIUser api;
    const std::string response = api.get_chat_completions(limit);
    const nlohmann::json results = parse_response(response);

    validation::is_list(results);

    ChatCompletions ccs;
    ccs.raw_response = response;

    unpack_chat_completions(results, ccs);
    return ccs;
}

bool delete_chat_completion(const std::string &chat_completion_id)
{
    OpenAIUser api;

    const std::string response = api.delete_chat_completion(chat_completion_id);
    const nlohmann::json results = parse_response(response);

    validation::is_chat_completion_deleted(results);
    return results["deleted"];
}
