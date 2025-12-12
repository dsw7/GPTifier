#include "embeddings.hpp"

#include "api_openai_user.hpp"
#include "ser_utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

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

Embedding create_embedding(const std::string &model, const std::string &input)
{
    const nlohmann::json data = { { "model", model }, { "input", input } };
    const auto result = networking::create_embedding(data.dump());

    if (not result) {
        throw_on_error_response(result.error().response);
    }

    return unpack_embedding(result->response, input);
}

} // namespace serialization
