#include "command_files.hpp"

#include "api_openai_user.hpp"
#include "cli.hpp"
#include "help_messages.hpp"
#include "models.hpp"
#include "parsers.hpp"
#include "utils.hpp"
#include "validation.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

void command_files_list(int argc, char **argv)
{
    bool print_raw_json = cli::get_opts_files_list(argc, argv);

    OpenAIUser api;
    const std::string response = api.get_uploaded_files();
    const json results = parse_response(response);

    if (print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    if (not validation::is_file_list(results)) {
        throw std::runtime_error("Response from OpenAI is not a file list");
    }

    print_sep();
    fmt::print("{:<30}{:<30}{:<30}{}\n", "File ID", "Filename", "Creation time", "Purpose");

    print_sep();
    std::vector<models::File> files;

    for (const auto &entry: results["data"]) {
        models::File file;
        file.created_at = entry["created_at"];
        file.filename = entry["filename"];
        file.id = entry["id"];
        file.purpose = entry["purpose"];
        files.push_back(file);
    }

    models::sort(files);

    for (auto it = files.begin(); it != files.end(); ++it) {
        it->print();
    }

    print_sep();
}

void delete_file(const std::string &file_id, json &results)
{
    OpenAIUser api;
    const std::string response = api.delete_file(file_id);
    results = parse_response(response);
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
        json results;

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
