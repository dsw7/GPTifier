#include "command_chats.hpp"

#include "cli.hpp"
#include "help_messages.hpp"
#include "serialization/chat_completions.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <string>
#include <vector>

namespace {

// List chats -----------------------------------------------------------------------------------------------

void print_chat_completions(const ChatCompletions &ccs)
{
    print_sep();
    fmt::print("{:<25}{:<40}{:<35}{}\n", "Created at", "Chat completion ID", "Prompt", "Completion");
    print_sep();

    for (const auto &it: ccs.completions) {
        const std::string dt_created_at = datetime_from_unix_timestamp(it.created);
        fmt::print("{:<25}{:<40}{:<35}{}\n", dt_created_at, it.id, it.prompt, it.completion);
    }

    print_sep();
}

void command_chats_list(int argc, char **argv)
{
    ParamsGetChatCompletions params = cli::get_opts_get_chat_completions(argc, argv);
    ChatCompletions ccs = get_chat_completions(std::get<int>(params.limit));

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
        cli::help_command_files_delete();
        return;
    }

    std::vector<std::string> args_or_ids;

    for (int i = 3; i < argc; i++) {
        args_or_ids.push_back(argv[i]);
    }

    if (args_or_ids[0] == "-h" or args_or_ids[0] == "--help") {
        cli::help_command_chats_delete();
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
        cli::help_command_chats();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "list") {
        command_chats_list(argc, argv);
    } else if (subcommand == "delete") {
        command_chats_delete(argc, argv);
    } else {
        cli::help_command_chats();
        exit(EXIT_FAILURE);
    }
}
