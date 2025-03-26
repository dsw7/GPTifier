#include "serialization/embeddings.hpp"

#include "networking/api_openai_user.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

Embedding unpack_response(const std::string &response)
{
    nlohmann::json results;
    Embedding embedding;

    try {
        results = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    try {
        embedding.embedding = results["data"][0]["embedding"].template get<std::vector<float>>();
        embedding.model = results["model"];
    } catch (nlohmann::json::out_of_range &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    } catch (nlohmann::json::type_error &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return embedding;
}

} // namespace

Embedding create_embedding(const std::string &model, const std::string &input)
{
    const nlohmann::json data = { { "model", model }, { "input", input } };

    OpenAIUser api;
    const std::string response = api.create_embedding(data.dump());
    Embedding embedding = unpack_response(response);

    embedding.input = input;
    return embedding;
}
