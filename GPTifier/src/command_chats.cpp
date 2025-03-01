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

    validation::is_list(results);

    print_sep();
    fmt::print("{:<25}{:<40}{:<35}{}\n", "Created at", "Chat completion ID", "Prompt", "Completion");
    print_sep();

    std::vector<models::ChatCompletion> chat_completions;

    for (const auto &entry: results["data"]) {
        validation::is_chat_completion(entry);

        models::ChatCompletion chat_completion;
        if (entry["metadata"].contains("prompt")) {
            chat_completion.prompt = entry["metadata"]["prompt"];
        }

        chat_completion.completion = entry["choices"][0]["message"]["content"];
        chat_completion.created = entry["created"];
        chat_completion.id = entry["id"];
        chat_completions.push_back(chat_completion);
    }

    for (const auto &it: chat_completions) {
        fmt::print("{:<25}{:<40}{:<35}{}\n",
            datetime_from_unix_timestamp(it.created), it.id, it.prompt, it.completion);
    }

    print_sep();
}

void delete_chat_completion(const std::string &chat_completion_id)
{
    OpenAIUser api;

    const std::string response = api.delete_chat_completion(chat_completion_id);
    const json results = parse_response(response);

    validation::is_chat_completion_deleted(results);
    const std::string id = results["id"];

    if (results["deleted"]) {
        fmt::print("Success! Deleted chat completion with ID: {}\n", id);
    } else {
        fmt::print("Warning! Did not delete chat completion with ID: {}\n", id);
    }
}

void command_chats_delete(int argc, char **argv)
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
        cli::help_command_chats_delete();
        return;
    }

    bool has_failed = false;

    for (auto it = args.begin(); it != args.end(); it++) {
        try {
            delete_chat_completion(*it);
        } catch (const std::runtime_error &e) {
            fmt::print(stderr, "Failed to delete chat completion with ID: {}. The error was: \"{}\"\n", *it, e.what());
            has_failed = true;
            continue;
        }
    }

    if (has_failed) {
        throw std::runtime_error("One or more failures occurred when deleting chat completions");
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
