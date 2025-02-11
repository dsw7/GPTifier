#include "command_files.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "help_messages.hpp"
#include "parsers.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

void delete_file(const std::string &file_id, json &results)
{
    Curl curl;
    const std::string response = curl.delete_file(file_id);
    results = parse_response(response);
}

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
    bool print_raw_json = cli::get_opts_files_list(argc, argv);

    Curl curl;
    const std::string response = curl.get_uploaded_files();

    if (print_raw_json) {
        print_raw_response(response);
        return;
    }

    const nlohmann::json results = parse_response(response);

    print_sep();
    fmt::print("{:<30}{:<30}{:<30}{}\n", "File ID", "Filename", "Creation time", "Purpose");

    print_sep();
    std::map<int, File> files;

    for (const auto &entry: results["data"]) {
        File file = { entry["filename"], entry["id"], entry["purpose"] };
        files.emplace(entry["created_at"], file);
    }

    for (auto it = files.begin(); it != files.end(); ++it) {
        print_file(it->first, it->second);
    }

    print_sep();
}

void command_files_delete(int argc, char **argv)
{
    if (argc < 4) {
        cli::help_command_files_delete();
        return;
    }

    std::vector<std::string> args;

    for (int i = 3; i < argc; i++) {
        args.push_back(argv[i]);
    }

    if (args[0] == "-h" or args[0] == "--help") {
        cli::help_command_files_delete();
        return;
    }

    bool has_failed = false;

    for (auto it = args.begin(); it != args.end(); it++) {
        nlohmann::json results;

        try {
            delete_file(*it, results);
        } catch (const std::runtime_error &e) {
            fmt::print(stderr, "Failed to delete file with ID: {}. The error was: \"{}\"\n", *it, e.what());
            has_failed = true;
            continue;
        }

        const std::string id = results["id"];

        if (results["deleted"]) {
            fmt::print("Success! Deleted file with ID: {}\n", id);
        } else {
            fmt::print("Warning! Did not delete file with ID: {}\n", id);
        }
    }

    if (has_failed) {
        throw std::runtime_error("One or more failures occurred when deleting files");
    }
}

} // namespace

void command_files(int argc, char **argv)
{
    if (argc == 2) {
        command_files_list(argc, argv);
        return;
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
