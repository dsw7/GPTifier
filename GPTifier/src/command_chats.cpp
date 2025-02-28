#include "command_chats.hpp"

#include "api_openai_user.hpp"
#include "cli.hpp"
#include "help_messages.hpp"
#include "models.hpp"
#include "parsers.hpp"
#include "utils.hpp"
#include "validation.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

void command_chats_list(int argc, char **argv)
{
    ParamsGetChatCompletions params = cli::get_opts_get_chat_completions(argc, argv);

    OpenAIUser api;
    const std::string response = api.get_chat_completions(std::get<int>(params.limit));
    const json results = parse_response(response);

    if (params.print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    if (not validation::is_chat_completions_list(results)) {
        throw std::runtime_error("Response from OpenAI is not a list of chat completions");
    }

    print_sep();
    fmt::print("{:<25}{:<40}{:<35}{}\n", "Created at", "Chat completion ID", "Prompt", "Completion");
    print_sep();

    std::vector<models::Completion> chat_completions;

    for (const auto &entry: results["data"]) {
        models::Completion chat_completion;
        if (entry["metadata"].contains("prompt")) {
            chat_completion.prompt = entry["metadata"]["prompt"];
        }

        chat_completion.completion = entry["choices"][0]["message"]["content"];
        chat_completion.created = entry["created"];
        chat_completion.id = entry["id"];
        chat_completions.push_back(chat_completion);
    }

    for (auto it = chat_completions.begin(); it != chat_completions.end(); ++it) {
        it->print();
    }

    print_sep();
}

void command_chats_delete()
{
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
        command_chats_delete();
    } else {
        cli::help_command_chats();
        exit(EXIT_FAILURE);
    }
}
