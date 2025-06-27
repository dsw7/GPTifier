#include "embeddings.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"

#include <json.hpp>

namespace serialization {

namespace {

void unpack_embedding(const nlohmann::json &json, Embedding &embedding)
{
    embedding.embedding = json["data"][0]["embedding"].template get<std::vector<float>>();
    embedding.model = json["model"];
}

} // namespace

Embedding create_embedding(const std::string &model, const std::string &input)
{
    const nlohmann::json data = { { "model", model }, { "input", input } };
    const std::string response = networking::create_embedding(data.dump());
    Embedding embedding = unpack_response<Embedding>(response, unpack_embedding);
    embedding.input = input;
    return embedding;
}

} // namespace serialization
