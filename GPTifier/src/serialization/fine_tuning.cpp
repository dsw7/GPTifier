#include "serialization/fine_tuning.hpp"

#include "networking/api_openai_user.hpp"
#include "parsers.hpp"
#include "validation.hpp"

#include <json.hpp>

std::string create_fine_tuning_job(const std::string &model, const std::string &training_file)
{
    const nlohmann::json data = { { "model", model }, { "training_file", training_file } };

    OpenAIUser api;
    const std::string response = api.create_fine_tuning_job(data.dump());
    const nlohmann::json results = parse_response(response);

    validation::is_fine_tuning_job(results);
    return results["id"];
}
