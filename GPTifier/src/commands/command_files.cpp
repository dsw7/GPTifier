#include "command_files.hpp"

#include "files.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void help_files()
{
    const std::string messages = R"(Manage files uploaded to OpenAI

Usage:
  gpt files [OPTION]
  gpt files list [OPTION]...
  gpt files delete [OPTION]
  gpt files delete FILE-ID...

Options:
  -h, --help  Print help information and exit

Commands:
  list        Get list of files uploaded to OpenAI servers
  delete      Delete one or more uploaded files
)";

    fmt::print("{}", messages);
}

void help_files_list()
{
    const std::string messages = R"(Get list of files uploaded to OpenAI servers

Usage:
  gpt files list [OPTION]...

Options:
  -h, --help  Print help information and exit
  -j, --json  Print raw JSON response from OpenAI
)";

    fmt::print("{}", messages);
}

void help_files_delete()
{
    const std::string messages = R"(Delete one or more uploaded files

Usage:
  gpt files delete [OPTION]
  gpt files delete FILE-ID...

Options:
  -h, --help  Print help information and exit
)";

    fmt::print("{}", messages);
}

// List files -----------------------------------------------------------------------------------------------

void print_files(const serialization::Files &files)
{
    utils::separator();
    fmt::print("{:<30}{:<30}{:<30}{}\n", "File ID", "Filename", "Creation time", "Purpose");
    utils::separator();

    for (const auto &file: files.files) {
        const std::string dt_created_at = utils::datetime_from_unix_timestamp(file.created_at);
        fmt::print("{:<30}{:<30}{:<30}{}\n", file.id, file.filename, dt_created_at, file.purpose);
    }

    utils::separator();
}

void list_files(int argc, char **argv)
{
    bool print_raw_json = false;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hj", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_files_list();
                exit(EXIT_SUCCESS);
            case 'j':
                print_raw_json = true;
                break;
            default:
                utils::exit_on_failure();
        }
    }

    const serialization::Files files = serialization::get_files();

    if (print_raw_json) {
        fmt::print("{}\n", files.raw_response);
        return;
    }

    print_files(files);
}

// Delete files ---------------------------------------------------------------------------------------------

bool loop_over_ids(const std::vector<std::string> &ids)
{
    bool success = true;

    for (const auto &id: ids) {
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

void delete_files(int argc, char **argv)
{
    if (argc == 3) {
        help_files_delete();
        return;
    }

    std::vector<std::string> args_or_ids;

    for (int i = 3; i < argc; i++) {
        args_or_ids.push_back(argv[i]);
    }

    if (args_or_ids[0] == "-h" or args_or_ids[0] == "--help") {
        help_files_delete();
        return;
    }

    if (not loop_over_ids(args_or_ids)) {
        throw std::runtime_error("One or more failures occurred when deleting files");
    }
}

} // namespace

namespace commands {

void command_files(int argc, char **argv)
{
    if (argc == 2) {
        list_files(argc, argv);
        return;
    }

    const std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        help_files();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "list") {
        list_files(argc, argv);
    } else if (subcommand == "delete") {
        delete_files(argc, argv);
    } else {
        help_files();
        exit(EXIT_FAILURE);
    }
}

} // namespace commands
