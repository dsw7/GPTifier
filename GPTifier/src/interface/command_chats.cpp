#include "interface/command_chats.hpp"

#include "chat_completions.hpp"
#include "interface/help_messages.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <string>
#include <vector>

namespace {

void help_chats()
{
    help::HelpMessages help;
    help.add_description("Manage chat completions uploaded to OpenAI.");
    help.add_synopsis("chats [-h | --help] (list | delete)");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_command("list", "List uploaded chat completions");
    help.add_command("delete", "Delete an uploaded chat completion");
    help.print();
}

void help_chats_list()
{
    help::HelpMessages help;
    help.add_description("List uploaded chat completions.");
    help.add_synopsis("chats list [-h | --help] [-j | --json] -l <limit> | --limit <limit>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.add_option("-l", "--limit", "Number of chat completions to show");
    help.print();
}

void help_chats_delete()
{
    help::HelpMessages help;
    help.add_description("Delete an uploaded chat completion.");
    help.add_synopsis("chats delete [-h | --help] <chat-completion-id...>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.print();
}

// List chats -----------------------------------------------------------------------------------------------

struct Params {
    bool print_raw_json = false;
    std::string limit = "20";
};

void read_cli_list_cc(int argc, char **argv, Params &params)
{
    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "limit", required_argument, 0, 'l' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hjl:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_chats_list();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 'l':
                params.limit = optarg;
                break;
            default:
                help::exit_on_failure();
        }
    };
}

void print_chat_completions(const ChatCompletions &ccs)
{
    utils::separator();
    fmt::print("{:<25}{:<40}{:<35}{}\n", "Created at", "Chat completion ID", "Prompt", "Completion");
    utils::separator();

    for (const auto &it: ccs.completions) {
        const std::string dt_created_at = utils::datetime_from_unix_timestamp(it.created);
        fmt::print("{:<25}{:<40}{:<35}{}\n", dt_created_at, it.id, it.prompt, it.completion);
    }

    utils::separator();
}

void command_chats_list(int argc, char **argv)
{
    Params params;
    read_cli_list_cc(argc, argv, params);

    int limit = utils::string_to_int(params.limit);
    if (limit < 1) {
        throw std::runtime_error("Limit must be greater than 0");
    }

    ChatCompletions ccs = get_chat_completions(limit);

    if (params.print_raw_json) {
        fmt::print("{}\n", ccs.raw_response);
        return;
    }

    print_chat_completions(ccs);
}

// Delete chat completion -----------------------------------------------------------------------------------

bool delete_chats(const std::vector<std::string> &ids)
{
    bool success = true;

    for (auto it: ids) {
        bool deleted = false;

        try {
            deleted = delete_chat_completion(it);
        } catch (const std::runtime_error &e) {
            fmt::print(stderr, "Failed to delete chat completion with ID: {}. The error was: \"{}\"\n", it, e.what());
            success = false;
            continue;
        }

        if (deleted) {
            fmt::print("Success! Deleted chat completion with ID: {}\n", it);
        } else {
            fmt::print("Warning! Did not delete chat completion with ID: {}\n", it);
        }
    }

    return success;
}

void command_chats_delete(int argc, char **argv)
{
    if (argc < 4) {
        help_chats_delete();
        return;
    }

    std::vector<std::string> args_or_ids;

    for (int i = 3; i < argc; i++) {
        args_or_ids.push_back(argv[i]);
    }

    if (args_or_ids[0] == "-h" or args_or_ids[0] == "--help") {
        help_chats_delete();
        return;
    }

    if (not delete_chats(args_or_ids)) {
        throw std::runtime_error("One or more failures occurred when deleting chats");
    }
}

} // namespace

void command_chats(int argc, char **argv)
{
    if (argc == 2) {
        command_chats_list(argc, argv);
        return;
    }

    const std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        help_chats();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "list") {
        command_chats_list(argc, argv);
    } else if (subcommand == "delete") {
        command_chats_delete(argc, argv);
    } else {
        help_chats();
        exit(EXIT_FAILURE);
    }
}
