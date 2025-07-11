#include "command_chats.hpp"

#include "chat_completions.hpp"
#include "help_messages.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <string>
#include <vector>

namespace {

void help_chats()
{
    const std::string messages = R"(Manage chat completions uploaded to OpenAI

Usage:
  gpt chats [OPTION]
  gpt chats list [OPTION]...
  gpt chats delete [OPTION]
  gpt chats delete CHAT-CMPL-ID...

Options:
  -h, --help  Print help information and exit

Commands:
  list        List uploaded chat completions
  delete      Delete one or more uploaded chat completions
)";

    fmt::print("{}", messages);
}

void help_chats_list()
{
    const std::string messages = R"(List uploaded chat completions

Usage:
  gpt chats list [OPTION]...

Options:
  -h, --help         Print help information and exit
  -j, --json         Print raw JSON response from OpenAI
  -l, --limit=LIMIT  Show LIMIT number of chat completions
)";

    fmt::print("{}", messages);
}

void help_chats_delete()
{
    const std::string messages = R"(Delete one or more uploaded chat completions

Usage:
  gpt chats delete [OPTION]
  gpt chats delete CHAT-CMPL-ID...

Options:
  -h, --help  Print help information and exit
)";

    fmt::print("{}", messages);
}

// List chats -----------------------------------------------------------------------------------------------

void loop_over_chat_completions(const serialization::ChatCompletions &ccs)
{
    utils::separator();
    fmt::print("{:<25}{:<40}{:<35}{}\n", "Created at", "Chat completion ID", "Prompt", "Completion");
    utils::separator();

    for (const auto &cc: ccs.completions) {
        const std::string dt_created_at = utils::datetime_from_unix_timestamp(cc.created);
        fmt::print("{:<25}{:<40}{:<35}{}\n", dt_created_at, cc.id, cc.prompt, cc.completion);
    }

    utils::separator();
}

void list_chat_completions(int argc, char **argv)
{
    bool print_raw_json = false;
    std::string limit = "20";

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
                print_raw_json = true;
                break;
            case 'l':
                limit = optarg;
                break;
            default:
                help::exit_on_failure();
        }
    };

    const int limit_i = utils::string_to_int(limit);

    if (limit_i < 1) {
        throw std::runtime_error("Limit must be greater than 0");
    }

    const serialization::ChatCompletions ccs = serialization::get_chat_completions(limit_i);

    if (print_raw_json) {
        fmt::print("{}\n", ccs.raw_response);
        return;
    }

    loop_over_chat_completions(ccs);
}

// Delete chat completion -----------------------------------------------------------------------------------

bool loop_over_ids(const std::vector<std::string> &ids)
{
    bool success = true;

    for (const auto &id: ids) {
        bool deleted = false;

        try {
            deleted = serialization::delete_chat_completion(id);
        } catch (const std::runtime_error &e) {
            fmt::print(stderr, "Failed to delete chat completion with ID: {}. The error was: \"{}\"\n", id, e.what());
            success = false;
            continue;
        }

        if (deleted) {
            fmt::print("Success! Deleted chat completion with ID: {}\n", id);
        } else {
            fmt::print("Warning! Did not delete chat completion with ID: {}\n", id);
        }
    }

    return success;
}

void delete_chat_completions(int argc, char **argv)
{
    if (argc == 3) {
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

    if (not loop_over_ids(args_or_ids)) {
        throw std::runtime_error("One or more failures occurred when deleting chats");
    }
}

} // namespace

namespace commands {

void command_chats(int argc, char **argv)
{
    if (argc == 2) {
        list_chat_completions(argc, argv);
        return;
    }

    const std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        help_chats();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "list") {
        list_chat_completions(argc, argv);
    } else if (subcommand == "delete") {
        delete_chat_completions(argc, argv);
    } else {
        help_chats();
        exit(EXIT_FAILURE);
    }
}

} // namespace commands
