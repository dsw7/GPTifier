#include "command_files.hpp"

#include "cli.hpp"
#include "help_messages.hpp"
#include "models.hpp"
#include "networking/api_openai_user.hpp"
#include "parsers.hpp"
#include "serialization/files.hpp"
#include "utils.hpp"
#include "validation.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

// List files -----------------------------------------------------------------------------------------------

void print_results(const Files &files)
{
    print_sep();
    fmt::print("{:<30}{:<30}{:<30}{}\n", "File ID", "Filename", "Creation time", "Purpose");
    print_sep();

    for (const auto &it: files.files) {
        const std::string dt_created_at = datetime_from_unix_timestamp(it.created_at);
        fmt::print("{:<30}{:<30}{:<30}{}\n", it.id, it.filename, dt_created_at, it.purpose);
    }

    print_sep();
}

void command_files_list(int argc, char **argv)
{
    bool print_raw_json = cli::get_opts_files_list(argc, argv);

    Files files = get_files();

    if (print_raw_json) {
        fmt::print("{}\n", files.raw_response);
        return;
    }

    print_results(files);
}

// Delete files ---------------------------------------------------------------------------------------------

void delete_file(const std::string &file_id)
{
    OpenAIUser api;

    const std::string response = api.delete_file(file_id);
    const json results = parse_response(response);

    validation::is_file(results);
    const std::string id = results["id"];

    if (results["deleted"]) {
        fmt::print("Success! Deleted file with ID: {}\n", id);
    } else {
        fmt::print("Warning! Did not delete file with ID: {}\n", id);
    }
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

    for (auto it: args) {
        try {
            delete_file(it);
        } catch (const std::runtime_error &e) {
            fmt::print(stderr, "Failed to delete file with ID: {}. The error was: \"{}\"\n", it, e.what());
            has_failed = true;
            continue;
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
