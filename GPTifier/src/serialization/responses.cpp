#include "responses.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"

#include <stdexcept>

namespace serialization {

namespace {

void unpack_response_(const nlohmann::json &json, Response &rp)
{
    if (json.contains("object")) {
        if (json["object"] != "response") {
            throw std::runtime_error("The response from OpenAI is not an OpenAI Response");
        }
    } else {
        throw std::runtime_error("The response from OpenAI does not contain an 'object' key");
    }

    bool job_complete = false;

    for (const auto &item: json["output"]) {
        if (item["type"] != "message") {
            continue;
        }

        if (item["status"] == "completed") {
            rp.output = item["content"][0]["text"];
            job_complete = true;
            break;
        }
    }

    if (not job_complete) {
        throw std::runtime_error("OpenAI did not complete the transaction");
    }

    rp.created = json["created_at"];
    rp.input_tokens = json["usage"]["input_tokens"];
    rp.model = json["model"];
    rp.output_tokens = json["usage"]["output_tokens"];
}

} // namespace

Response create_response(const std::string &input, const std::string &model, float temp)
{
    const nlohmann::json data = {
        { "input", input },
        { "model", model },
        { "store", false },
        { "temperature", temp },
    };

    const auto start = std::chrono::high_resolution_clock::now();
    const std::string response = networking::create_response(data.dump());
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> rtt = end - start;

    Response rp = unpack_response<Response>(response, unpack_response_);
    rp.input = input;
    rp.raw_response = response;
    rp.rtt = rtt;
    return rp;
}

} // namespace serialization
