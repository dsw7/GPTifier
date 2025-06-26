#include "files.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

void unpack_files(const nlohmann::json &json, Files &files)
{
    for (const auto &entry: json["data"]) {
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
    const nlohmann::json json = response_to_json(response);
    Files files;

    try {
        unpack_files(json, files);
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
    const std::string response = networking::get_uploaded_files();
    Files files = unpack_response(response);
    files.raw_response = response;
    return files;
}

bool delete_file(const std::string &file_id)
{
    const std::string response = networking::delete_file(file_id);
    const nlohmann::json json = response_to_json(response);

    if (not json.contains("deleted")) {
        throw std::runtime_error("Malformed response. Missing 'deleted' key");
    }

    return json["deleted"];
}

std::string upload_file(const std::string &filename)
{
    const std::string purpose = "fine-tune";
    const std::string response = networking::upload_file(filename, purpose);
    const nlohmann::json json = response_to_json(response);

    if (not json.contains("id")) {
        throw std::runtime_error("Malformed response. Missing 'id' key");
    }

    return json["id"];
}

} // namespace serialization
