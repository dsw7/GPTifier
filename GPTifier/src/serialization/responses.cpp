#include "responses.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"

#include <stdexcept>

namespace serialization {

namespace {

void unpack_response_(const nlohmann::json &json, Response &rp)
{
    bool found_end = false;

    for (const auto &step: json["output"]) {
        if (not step.contains("status")) {
            continue;
        }

        if (step["status"] != "completed") {
            continue;
        }

        if (step["type"] == "message") {
            rp.output = step["content"][0]["text"];
            found_end = true;
        }
    }

    if (not found_end) {
        throw std::runtime_error("Something is wrong. Could not unpack message");
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
