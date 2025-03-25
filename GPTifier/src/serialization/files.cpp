#include "serialization/files.hpp"

#include "networking/api_openai_user.hpp"
#include "parsers.hpp"
#include "validation.hpp"

#include <json.hpp>

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
