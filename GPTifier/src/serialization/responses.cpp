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

    nlohmann::json content;
    bool job_complete = false;

    for (const auto &item: json["output"]) {
        if (item["type"] != "message") {
            continue;
        }

        if (item["status"] == "completed") {
            content = item["content"][0];
            job_complete = true;
            break;
        }
    }

    if (not job_complete) {
        throw std::runtime_error("OpenAI did not complete the transaction");
    }

    if (content["type"] == "output_text") {
        rp.output = content["text"];
    } else if (content["type"] == "refusal") {
        rp.output = content["refusal"];
    } else {
        throw std::runtime_error("Some unknown object type was returned from OpenAI");
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

std::string test_curl_handle_is_reusable()
{
    static std::string low_cost_model = "gpt-3.5-turbo";
    const nlohmann::json data = {
        { "input", "What is 5 + 5? Return the result as follows: >>>{result}<<<" },
        { "model", low_cost_model },
        { "store", false },
        { "temperature", 0.00 },
    };

    const std::string dump = data.dump();
    const std::string response_1 = networking::create_response(dump);
    const std::string response_2 = networking::create_response(dump);
    const std::string response_3 = networking::create_response(dump);

    const Response rp_1 = unpack_response<Response>(response_1, unpack_response_);
    const Response rp_2 = unpack_response<Response>(response_2, unpack_response_);
    const Response rp_3 = unpack_response<Response>(response_3, unpack_response_);

    const nlohmann::json results = {
        { "result_1", rp_1.output },
        { "result_2", rp_2.output },
        { "result_3", rp_3.output },
    };

    return results.dump(4);
}

} // namespace serialization
