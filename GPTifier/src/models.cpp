#include "models.hpp"

namespace models {

nlohmann::json ChatCompletion::jsonify() const
{
    nlohmann::json results;
    results["completion"] = this->completion;
    results["completion_tokens"] = this->completion_tokens;
    results["created"] = this->created;
    results["id"] = this->id;
    results["model"] = this->model;
    results["prompt"] = this->prompt;
    results["prompt_tokens"] = this->prompt_tokens;

    return results;
}

nlohmann::json Embedding::jsonify() const
{
    nlohmann::json results;
    results["embedding"] = this->embedding;
    results["input"] = this->input;
    results["model"] = this->model;

    return results;
}

} // namespace models
