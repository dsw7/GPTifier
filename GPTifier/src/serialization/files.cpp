#include "serialization/files.hpp"

#include "networking/api_openai_user.hpp"
#include "serialization/parse_response.hpp"
#include "serialization/validation.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace {

void unpack_files(const nlohmann::json &results, Files &files)
{
    for (const auto &entry: results["data"]) {
        validation::is_file(entry);

        File file;
        file.created_at = entry["created_at"];
        file.filename = entry["filename"];
        file.id = entry["id"];
        file.purpose = entry["purpose"];

        files.files.push_back(file);
    }
}

} // namespace

Files get_files()
{
    OpenAIUser api;
    const std::string response = api.get_uploaded_files();
    const nlohmann::json results = parse_response(response);

    validation::is_list(results);

    Files files;
    files.raw_response = response;

    unpack_files(results, files);
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
    const nlohmann::json results = parse_response(response);

    validation::is_file(results);
    return results["id"];
}
