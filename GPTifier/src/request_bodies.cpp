#include "request_bodies.hpp"

#include <json.hpp>

std::string get_chat_completion_request_body(const std::string &model, const std::string &prompt, float temperature)
{
    nlohmann::json messages = {{"role", "user"}, {"content", prompt}};
    nlohmann::json body = {
        {"model", model}, {"temperature", temperature}, {"messages", nlohmann::json::array({messages})}};

    return body.dump(2);
}

std::string get_embedding_request_body(const std::string &model, const std::string &input)
{
    nlohmann::json body = {{"model", model}, {"input", input}};
    return body.dump(2);
}
