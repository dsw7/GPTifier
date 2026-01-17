#include "embeddings.hpp"

#include "api_ollama.hpp"
#include "api_openai_user.hpp"
#include "ser_utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

Embedding unpack_openai_embedding_(const std::string &response, const std::string &input)
{
    const nlohmann::json json = parse_json(response);
    Embedding embedding;

    try {
        embedding.embedding = json["data"][0]["embedding"].template get<std::vector<float>>();
        embedding.model = json["model"];
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    embedding.input = input;
    embedding.source = "OpenAI";
    return embedding;
}

Embedding unpack_ollama_embedding_(const std::string &response, const std::string &input)
{
    const nlohmann::json json = parse_json(response);
    Embedding embedding;

    try {
        embedding.embedding = json["embeddings"][0].template get<std::vector<float>>();
        embedding.model = json["model"];
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    embedding.input = input;
    embedding.source = "Ollama";
    return embedding;
}

} // namespace

Embedding create_openai_embedding(const std::string &model, const std::string &input)
{
    const nlohmann::json data = { { "model", model }, { "input", input } };
    const auto result = networking::create_openai_embedding(data.dump());

    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    return unpack_openai_embedding_(result->response, input);
}

Embedding create_ollama_embedding(const std::string &model, const std::string &input)
{
    const nlohmann::json data = { { "model", model }, { "input", input } };
    const auto result = networking::create_ollama_embedding(data.dump());

    if (not result) {
        throw_on_ollama_error_response(result.error().response);
    }

    return unpack_ollama_embedding_(result->response, input);
}

} // namespace serialization
