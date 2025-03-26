#include "serialization/embeddings.hpp"

#include "networking/api_openai_user.hpp"
#include "serialization/parse_response.hpp"
#include "serialization/validation.hpp"

#include <json.hpp>

namespace {

void unpack_response(const nlohmann::json &response, Embedding &embedding)
{
    if (response["object"] != "list") {
        throw std::runtime_error("Object is not a list of embeddings");
    }

    if (response["data"].empty()) {
        throw std::runtime_error("List of embeddings from OpenAI is empty");
    }

    if (response["data"][0] != "embedding") {
        throw std::runtime_error("Object is not an embedding");
    }

    embedding.embedding = response["data"][0]["embedding"].template get<std::vector<float>>();
    embedding.model = response["model"];
}

} // namespace

Embedding create_embedding(const std::string &model, const std::string &input)
{
    const nlohmann::json data = { { "model", model }, { "input", input } };

    OpenAIUser api;
    const std::string response = api.create_embedding(data.dump());
    const nlohmann::json results = parse_response(response);

    Embedding embedding;
    unpack_response(results, embedding);

    embedding.input = input;
    return embedding;
}
