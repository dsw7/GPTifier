#include "command_files.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "help_messages.hpp"
#include "json.hpp"
#include "parsers.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <iostream>
#include <map>
#include <optional>
#include <string>

namespace {

struct File {
    std::string filename;
    std::string id;
    std::string purpose;
};

void print_file(int created_at, const File &file)
{
    const std::string datetime = datetime_from_unix_timestamp(created_at);
    fmt::print("{:<30}{:<30}{:<30}{}\n", file.id, file.filename, datetime, file.purpose);
}

void command_files_list(int argc, char **argv)
{
    if (argc >= 4) {
        const std::string option = argv[3];

        if (option == "-h" or option == "--help") {
            cli::help_command_files_list();
            return;
        }
    }

    const std::string response = query_list_files_api();
    const std::optional<nlohmann::json> results = parse_response(response);

    if (not results.has_value()) {
        return;
    }

    reporting::print_sep();
    fmt::print("{:<30}{:<30}{:<30}{}\n", "File ID", "Filename", "Creation time", "Purpose");

    reporting::print_sep();
    std::map<int, File> files = {};

    for (const auto &entry: results.value()["data"]) {
        File file;
        file.id = entry["id"];
        file.filename = entry["filename"];
        file.purpose = entry["purpose"];
        files[entry["created_at"]] = file;
    }

    for (auto it = files.begin(); it != files.end(); ++it) {
        print_file(it->first, it->second);
    }

    reporting::print_sep();
}

void command_files_delete(int argc, char **argv)
{
    if (argc < 4) {
        cli::help_command_files_delete();
        return;
    }

    const std::string opt_or_file_id = argv[3];

    if (opt_or_file_id == "-h" or opt_or_file_id == "--help") {
        cli::help_command_files_delete();
        return;
    }

    const std::string response = query_delete_file_api(opt_or_file_id);
    const std::optional<nlohmann::json> results = parse_response(response);

    if (not results.has_value()) {
        return;
    }

    const std::string id = results.value()["id"];

    if (results.value()["deleted"]) {
        fmt::print("Success!\nDeleted file with ID: {}\n", id);
    } else {
        fmt::print("Warning!\nDid not delete file with ID: {}\n", id);
    }
}

} // namespace

void command_files(int argc, char **argv)
{
    if (argc < 3) {
        cli::help_command_files();
        exit(EXIT_FAILURE);
    }

    std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        cli::help_command_files();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "list") {
        command_files_list(argc, argv);
    } else if (subcommand == "delete") {
        command_files_delete(argc, argv);
    } else {
        cli::help_command_files();
        exit(EXIT_FAILURE);
    }
}
