#include "command_files.hpp"

#include "files.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void help_files_()
{
    const std::string messages = R"(Manage files uploaded to OpenAI servers.

Usage:
  gpt files [OPTIONS] COMMAND [ARGS]...

Options:
  -h, --help  Print help information and exit

Commands:
  list        Get list of files uploaded to OpenAI servers
  delete      Delete one or more uploaded files
)";

    fmt::print("{}\n", messages);
}

void help_files_list_()
{
    const std::string messages = R"(Get list of files uploaded to OpenAI servers.

Usage:
  gpt files list [OPTIONS]

Options:
  -h, --help  Print help information and exit
  -j, --json  Print raw JSON response from OpenAI
)";

    fmt::print("{}\n", messages);
}

void help_files_delete_()
{
    const std::string messages = R"(Delete one or more uploaded files.

Usage:
  gpt files delete [OPTIONS] FILE-ID...

Options:
  -h, --help  Print help information and exit

Use "gpt files list" to get the IDs corresponding to files to be deleted
)";

    fmt::print("{}\n", messages);
}

// List files -----------------------------------------------------------------------------------------------

using serialization::Files;

void print_uploaded_files_(const Files &files)
{
    fmt::print("{:<30}{:<30}{:<30}{}\n", "File ID", "Filename", "Creation time", "Purpose");

    for (const auto &file: files.files) {
        const std::string dt_created_at = utils::datetime_from_unix_timestamp(file.created_at);
        fmt::print("{:<30}{:<30}{:<30}{}\n", file.id, file.filename, dt_created_at, file.purpose);
    }
}

void list_uploaded_files_(const int argc, char **argv)
{
    bool print_raw_json = false;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hj", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_files_list_();
                exit(EXIT_SUCCESS);
            case 'j':
                print_raw_json = true;
                break;
            default:
                utils::exit_on_failure();
        }
    }

    const Files files = serialization::get_files();

    if (print_raw_json) {
        fmt::print("{}\n", files.raw_response);
        return;
    }

    print_uploaded_files_(files);
}

// Delete files ---------------------------------------------------------------------------------------------

bool loop_over_file_ids_to_delete_(const std::vector<std::string> &ids)
{
    bool success = true;

    for (const auto &id: ids) {
        if (id.empty()) {
            fmt::print(stderr, "Cannot delete file. ID is empty\n");
            success = false;
            continue;
        }

        bool deleted = false;

        try {
            deleted = serialization::delete_file(id);
        } catch (const std::runtime_error &e) {
            fmt::print(stderr, "Failed to delete file with ID: {}. The error was: \"{}\"\n", id, e.what());
            success = false;
            continue;
        }

        if (deleted) {
            fmt::print("Success! Deleted file with ID: {}\n", id);
        } else {
            fmt::print("Warning! Did not delete file with ID: {}\n", id);
        }
    }

    return success;
}

void delete_uploaded_files_(const int argc, char **argv)
{
    if (argc == 3) {
        throw std::runtime_error("One or more file IDs need to be provided");
    }

    std::vector<std::string> args_or_ids;

    for (int i = 3; i < argc; i++) {
        args_or_ids.push_back(argv[i]);
    }

    if (args_or_ids[0] == "-h" or args_or_ids[0] == "--help") {
        help_files_delete_();
        return;
    }

    if (not loop_over_file_ids_to_delete_(args_or_ids)) {
        throw std::runtime_error("One or more failures occurred when deleting files");
    }
}

} // namespace

namespace commands {

void command_files(const int argc, char **argv)
{
    if (argc == 2) {
        list_uploaded_files_(argc, argv);
        return;
    }

    const std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        help_files_();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "list") {
        list_uploaded_files_(argc, argv);
    } else if (subcommand == "delete") {
        delete_uploaded_files_(argc, argv);
    } else {
        help_files_();
        exit(EXIT_FAILURE);
    }
}

} // namespace commands
