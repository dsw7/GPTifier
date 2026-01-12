#include "responses.hpp"

#include "api_openai_user.hpp"
#include "ser_utils.hpp"

#include <stdexcept>

namespace serialization {

namespace {

Response unpack_response(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

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

    Response rp;

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
    return rp;
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
    const auto result = networking::create_response(data.dump());
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> rtt = end - start;

    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    Response rp = unpack_response(result->response);
    rp.input = input;
    rp.raw_response = result->response;
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

    const auto result_1 = networking::create_response(dump);
    if (not result_1) {
        throw_on_openai_error_response(result_1.error().response);
    }

    const auto result_2 = networking::create_response(dump);
    if (not result_2) {
        throw_on_openai_error_response(result_2.error().response);
    }

    const auto result_3 = networking::create_response(dump);
    if (not result_3) {
        throw_on_openai_error_response(result_3.error().response);
    }

    const Response rp_1 = unpack_response(result_1->response);
    const Response rp_2 = unpack_response(result_2->response);
    const Response rp_3 = unpack_response(result_3->response);

    const nlohmann::json results = {
        { "result_1", rp_1.output },
        { "result_2", rp_2.output },
        { "result_3", rp_3.output },
    };

    return results.dump(4);
}

} // namespace serialization
