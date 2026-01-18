#include "files.hpp"

#include "api_openai_user.hpp"
#include "ser_utils.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

File unpack_file_object_(const nlohmann::json &entry)
{
    File file_obj;
    file_obj.created_at = entry["created_at"];
    file_obj.created_at_dt_str = utils::datetime_from_unix_timestamp(entry["created_at"]);
    file_obj.filename = entry["filename"];
    file_obj.id = entry["id"];
    file_obj.purpose = entry["purpose"];
    return file_obj;
}

Files unpack_files_response_(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    Files files;
    files.raw_response = response;

    try {
        for (const auto &entry: json["data"]) {
            files.files.push_back(unpack_file_object_(entry));
        }
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return files;
}

} // namespace

Files get_files()
{
    const auto result = networking::get_uploaded_files();

    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    return unpack_files_response_(result->response);
}

bool delete_file(const std::string &file_id)
{
    const auto result = networking::delete_file(file_id);
    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    const nlohmann::json json = parse_json(result->response);

    if (not json.contains("deleted")) {
        throw std::runtime_error("Malformed response. Missing 'deleted' key");
    }

    return json["deleted"];
}

std::string upload_file(const std::string &filename)
{
    const std::string purpose = "fine-tune";

    const auto result = networking::upload_file(filename, purpose);
    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    const nlohmann::json json = parse_json(result->response);

    if (not json.contains("id")) {
        throw std::runtime_error("Malformed response. Missing 'id' key");
    }

    return json["id"];
}

} // namespace serialization
