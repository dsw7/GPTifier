#include "embeddings.hpp"

#include "api_openai_user.hpp"

#include <json.hpp>
#include <fmt/core.h>

namespace serialization {

namespace {

Embedding unpack_embedding(const std::string &response, const std::string &input)
{
    const nlohmann::json json = parse_json(response);

    Embedding embedding;
    embedding.input = input;

    try {
        embedding.embedding = json["data"][0]["embedding"].template get<std::vector<float>>();
        embedding.model = json["model"];
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack embeddings response: {}", e.what()));
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
