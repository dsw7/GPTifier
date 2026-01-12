#include "responses.hpp"

#include "api_ollama.hpp"
#include "api_openai_user.hpp"
#include "ser_utils.hpp"

#include <stdexcept>

namespace serialization {

namespace {

OpenAIResponse unpack_openai_response(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    if (json.contains("object")) {
        if (json["object"] != "response") {
            throw std::runtime_error("The response from OpenAI is not an OpenAI Response");
        }
    } else {
        throw std::runtime_error("The response from OpenAI does not contain an 'object' key");
    }

    nlohmann::json content;
    bool job_complete = false;

    for (const auto &item: json["output"]) {
        if (item["type"] != "message") {
            continue;
        }

        if (item["status"] == "completed") {
            content = item["content"][0];
            job_complete = true;
            break;
        }
    }

    if (not job_complete) {
        throw std::runtime_error("OpenAI did not complete the transaction");
    }

    OpenAIResponse rp;

    if (content["type"] == "output_text") {
        rp.output = content["text"];
    } else if (content["type"] == "refusal") {
        rp.output = content["refusal"];
    } else {
        throw std::runtime_error("Some unknown object type was returned from OpenAI");
    }

    rp.created = json["created_at"];
    rp.input_tokens = json["usage"]["input_tokens"];
    rp.model = json["model"];
    rp.output_tokens = json["usage"]["output_tokens"];
    return rp;
}

OllamaResponse unpack_ollama_response(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    if (not json.contains("done")) {
        throw std::runtime_error("The response from Ollama does not contain the 'done' key");
    }

    if (not json["done"]) {
        throw std::runtime_error("The response from Ollama indicates the job is not done");
    }

    OllamaResponse results;
    results.created = json["created_at"];
    results.input_tokens = json["prompt_eval_count"];
    results.model = json["model"];
    results.output = json["response"];
    results.output_tokens = json["eval_count"];
    return results;
}

} // namespace

OpenAIResponse create_openai_response(const std::string &input, const std::string &model, float temp)
{
    const nlohmann::json data = {
        { "input", input },
        { "model", model },
        { "store", false },
        { "temperature", temp },
    };

    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = networking::create_openai_response(data.dump());
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> rtt = end - start;

    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    OpenAIResponse rp = unpack_openai_response(result->response);
    rp.input = input;
    rp.raw_response = result->response;
    rp.rtt = rtt;
    return rp;
}

OllamaResponse create_ollama_response(const std::string &prompt, const std::string &model)
{
    const nlohmann::json data = {
        { "model", model },
        { "prompt", prompt },
        { "stream", false },
    };

    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = networking::generate_ollama_response(data.dump());
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> rtt = end - start;

    if (not result) {
        throw_on_ollama_error_response(result.error().response);
    }

    OllamaResponse rp = unpack_ollama_response(result->response);
    rp.input = prompt;
    rp.raw_response = result->response;
    rp.rtt = rtt;
    return rp;
}

std::string test_curl_handle_is_reusable()
{
    static std::string low_cost_model = "gpt-3.5-turbo";
    const nlohmann::json data = {
        { "input", "What is 5 + 5? Return the result as follows: >>>{result}<<<" },
        { "model", low_cost_model },
        { "store", false },
        { "temperature", 0.00 },
    };

    const std::string dump = data.dump();

    const auto result_1 = networking::create_openai_response(dump);
    if (not result_1) {
        throw_on_openai_error_response(result_1.error().response);
    }

    const auto result_2 = networking::create_openai_response(dump);
    if (not result_2) {
        throw_on_openai_error_response(result_2.error().response);
    }

    const auto result_3 = networking::create_openai_response(dump);
    if (not result_3) {
        throw_on_openai_error_response(result_3.error().response);
    }

    const OpenAIResponse rp_1 = unpack_openai_response(result_1->response);
    const OpenAIResponse rp_2 = unpack_openai_response(result_2->response);
    const OpenAIResponse rp_3 = unpack_openai_response(result_3->response);

    const nlohmann::json results = {
        { "result_1", rp_1.output },
        { "result_2", rp_2.output },
        { "result_3", rp_3.output },
    };

    return results.dump(4);
}

} // namespace serialization
