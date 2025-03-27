#include "serialization/embeddings.hpp"

#include "networking/api_openai_user.hpp"
#include "serialization/response_to_json.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

Embedding unpack_response(const std::string &response)
{
    Embedding embedding;
    const nlohmann::json json = response_to_json(response);

    try {
        embedding.embedding = json["data"][0]["embedding"].template get<std::vector<float>>();
        embedding.model = json["model"];
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
