#include "embeddings.hpp"

#include "api_openai_user.hpp"

#include <json.hpp>

namespace serialization {

namespace {

Embedding unpack_embedding(const std::string &response, const std::string &input)
{
    const nlohmann::json json = parse_json(response);

    Embedding embedding;
    embedding.input = input;

    if (json.contains("data") && json["data"].is_array() && not json["data"].empty()) {
        if (json["data"][0].contains("embedding")) {
            embedding.embedding = json["data"][0]["embedding"].template get<std::vector<float>>();
        } else {
            throw std::runtime_error("JSON data does not contain the expected 'embedding'");
        }
    } else {
        throw std::runtime_error("JSON does not contain the expected 'data'");
    }

    if (json.contains("model")) {
        embedding.model = json["model"];
    } else {
        throw std::runtime_error("JSON does not contain the 'model'");
    }

    return embedding;
}

} // namespace

EmbedResult create_embedding(const std::string &model, const std::string &input)
{
    const nlohmann::json data = { { "model", model }, { "input", input } };
    const auto result = networking::create_embedding(data.dump());

    if (result.has_value()) {
        return unpack_embedding(result.value().response, input);
    }

    return std::unexpected(unpack_error(result.error().response, result.error().code));
}

} // namespace serialization
