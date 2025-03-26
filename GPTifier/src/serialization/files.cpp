#include "serialization/files.hpp"

#include "networking/api_openai_user.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

void unpack_files(const nlohmann::json &results, Files &files)
{
    for (const auto &entry: results["data"]) {
        File file;
        file.created_at = entry["created_at"];
        file.filename = entry["filename"];
        file.id = entry["id"];
        file.purpose = entry["purpose"];

        files.files.push_back(file);
    }
}

Files unpack_response(const std::string &response)
{
    Files files;
    nlohmann::json results;

    try {
        results = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    try {
        unpack_files(results, files);
    } catch (nlohmann::json::out_of_range &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    } catch (nlohmann::json::type_error &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return files;
}

} // namespace

Files get_files()
{
    OpenAIUser api;
    const std::string response = api.get_uploaded_files();

    Files files = unpack_response(response);
    files.raw_response = response;
    return files;
}

bool delete_file(const std::string &file_id)
{
    OpenAIUser api;
    const std::string response = api.delete_file(file_id);

    nlohmann::json results;
    try {
        results = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    if (not results.contains("deleted")) {
        throw std::runtime_error("Malformed response. Missing 'deleted' key");
    }

    return results["deleted"];
}

std::string upload_file(const std::string &filename)
{
    OpenAIUser api;
    const std::string purpose = "fine-tune";
    const std::string response = api.upload_file(filename, purpose);

    nlohmann::json results;
    try {
        results = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse response: {}", e.what()));
    }

    if (not results.contains("id")) {
        throw std::runtime_error("Malformed response. Missing 'id' key");
    }

    return results["id"];
}
