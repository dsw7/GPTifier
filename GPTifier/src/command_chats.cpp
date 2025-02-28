#include "command_chats.hpp"

#include "help_messages.hpp"

#include <string>

namespace {

void command_chats_list()
{
}

void command_chats_delete()
{
}

} // namespace

void command_chats(int argc, char **argv)
{
    if (argc == 2) {
        command_chats_list();
        return;
    }

    const std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        cli::help_command_chats();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "list") {
        command_chats_list();
    } else if (subcommand == "delete") {
        command_chats_delete();
    } else {
        cli::help_command_chats();
        exit(EXIT_FAILURE);
    }
}
