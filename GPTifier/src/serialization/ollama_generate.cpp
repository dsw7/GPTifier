#include "ollama_generate.hpp"

#include "api_ollama.hpp"
#include "ser_utils.hpp"

#include <stdexcept>

namespace serialization {

namespace {

OllamaResponse deserialize_ollama_response(const std::string &response)
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

    OllamaResponse rp = deserialize_ollama_response(result->response);
    rp.input = prompt;
    rp.raw_response = result->response;
    rp.rtt = rtt;
    return rp;
}

} // namespace serialization
