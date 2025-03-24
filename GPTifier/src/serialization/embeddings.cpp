#include "serialization/embeddings.hpp"

#include "networking/api_openai_user.hpp"
#include "parsers.hpp"
#include "validation.hpp"

using json = nlohmann::json;

Embedding create_embedding(const std::string &model, const std::string &input)
{
    const json data = { { "model", model }, { "input", input } };

    OpenAIUser api;
    const std::string response = api.create_embedding(data.dump());

    const json results = parse_response(response);
    validation::is_list(results);

    if (validation::is_list_empty(results)) {
        throw std::runtime_error("List of embeddings from OpenAI is empty");
    }

    const json first_embedding = results["data"][0];
    validation::is_embedding(first_embedding);

    Embedding em;

    em.embedding = first_embedding["embedding"].template get<std::vector<float>>();
    em.input = input;
    em.model = results["model"];

    return em;
}
