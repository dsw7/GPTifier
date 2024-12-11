#include "command_files.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "help_messages.hpp"
#include "json.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <iostream>
#include <map>
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

    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error")) {
        const std::string error = results["error"]["message"];
        reporting::print_error(error);
        return;
    }

    reporting::print_sep();
    fmt::print("{:<30}{:<30}{:<30}{}\n", "File ID", "Filename", "Creation time", "Purpose");

    reporting::print_sep();
    std::map<int, File> files = {};

    for (const auto &entry: results["data"]) {
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

void command_files_upload(int argc, char **argv)
{
    if (argc < 4) {
        cli::help_command_files_upload();
        return;
    }

    const std::string opt_or_filename = argv[3];

    if (opt_or_filename == "-h" or opt_or_filename == "--help") {
        cli::help_command_files_upload();
        return;
    }

    const std::string response = query_upload_file_api(opt_or_filename);
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error")) {
        const std::string error = results["error"]["message"];
        reporting::print_error(error);
        return;
    }

    fmt::print("Success!\nUploaded file: {}\nWith ID: {}\n", results["filename"], results["id"]);
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
    } else if (subcommand == "upload") {
        command_files_upload(argc, argv);
    } else {
        cli::help_command_files();
        exit(EXIT_FAILURE);
    }
}
