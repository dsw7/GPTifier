#include "interface/command_files.hpp"

#include "interface/help_messages.hpp"
#include "serialization/files.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void help_files()
{
    help::HelpMessages help;
    help.add_description("Manage files uploaded to OpenAI.");
    help.add_synopsis("files [OPTIONS]");
    help.add_synopsis("files list [OPTIONS]");
    help.add_synopsis("files delete [OPTIONS] FILE-ID...");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_command("list", "List uploaded files");
    help.add_command("delete", "Delete an uploaded file");
    help.print();
}

void help_files_list()
{
    help::HelpMessages help;
    help.add_description("List uploaded files.");
    help.add_synopsis("files list [OPTIONS]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.print();
}

void help_files_delete()
{
    help::HelpMessages help;
    help.add_description("Delete an uploaded file.");
    help.add_synopsis("files delete [OPTIONS] FILE-ID...");
    help.add_option("-h", "--help", "Print help information and exit");
    help.print();
}

// List files -----------------------------------------------------------------------------------------------

bool read_cli(int argc, char **argv)
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
                help::exit_on_failure();
        }
    }

    return print_raw_json;
}

void print_files(const Files &files)
{
    utils::separator();
    fmt::print("{:<30}{:<30}{:<30}{}\n", "File ID", "Filename", "Creation time", "Purpose");
    utils::separator();

    for (const auto &it: files.files) {
        const std::string dt_created_at = utils::datetime_from_unix_timestamp(it.created_at);
        fmt::print("{:<30}{:<30}{:<30}{}\n", it.id, it.filename, dt_created_at, it.purpose);
    }

    utils::separator();
}

void command_files_list(int argc, char **argv)
{
    bool print_raw_json = read_cli(argc, argv);

    Files files = get_files();

    if (print_raw_json) {
        fmt::print("{}\n", files.raw_response);
        return;
    }

    print_files(files);
}

// Delete files ---------------------------------------------------------------------------------------------

bool delete_files(const std::vector<std::string> &ids)
{
    bool success = true;

    for (auto it: ids) {
        bool deleted = false;

        try {
            deleted = delete_file(it);
        } catch (const std::runtime_error &e) {
            fmt::print(stderr, "Failed to delete file with ID: {}. The error was: \"{}\"\n", it, e.what());
            success = false;
            continue;
        }

        if (deleted) {
            fmt::print("Success! Deleted file with ID: {}\n", it);
        } else {
            fmt::print("Warning! Did not delete file with ID: {}\n", it);
        }
    }

    return success;
}

void command_files_delete(int argc, char **argv)
{
    if (argc < 4) {
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

    if (not delete_files(args_or_ids)) {
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
        help_files();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "list") {
        command_files_list(argc, argv);
    } else if (subcommand == "delete") {
        command_files_delete(argc, argv);
    } else {
        help_files();
        exit(EXIT_FAILURE);
    }
}
