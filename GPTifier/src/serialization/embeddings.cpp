#include "serialization/embeddings.hpp"

#include "networking/api_openai_user.hpp"
#include "serialization/parse_response.hpp"
#include "serialization/validation.hpp"

#include <json.hpp>

Embedding create_embedding(const std::string &model, const std::string &input)
{
    const nlohmann::json data = { { "model", model }, { "input", input } };

    OpenAIUser api;
    const std::string response = api.create_embedding(data.dump());
    const nlohmann::json results = parse_response(response);

    validation::is_list(results);

    if (validation::is_list_empty(results)) {
        throw std::runtime_error("List of embeddings from OpenAI is empty");
    }

    const nlohmann::json first_embedding = results["data"][0];
    validation::is_embedding(first_embedding);

    Embedding em;

    em.embedding = first_embedding["embedding"].template get<std::vector<float>>();
    em.input = input;
    em.model = results["model"];

    return em;
}
