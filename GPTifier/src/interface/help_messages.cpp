#include "interface/help_messages.hpp"

#include "utils.hpp"

#include <fmt/core.h>
#include <string>
#include <vector>

namespace {

const std::string ws_2 = std::string(2, ' ');
const std::string ws_4 = std::string(4, ' ');

void print_bash_block(const std::string &command)
{
    fmt::print(fg(green), "{0}```bash\n{0}{1}\n{0}```\n", ws_2, command);
}

struct Option {
    std::string description;
    std::string opt_long;
    std::string opt_short;
};

struct Command {
    std::string description;
    std::string name;
};

struct Example {
    std::string command;
    std::string description;
};

class HelpMessages {
private:
    void print_commands();
    void print_description();
    void print_examples();
    void print_name_version();
    void print_options();
    void print_synopsis();

    bool print_metadata = false;
    std::vector<Command> commands;
    std::vector<Example> examples;
    std::vector<Option> options;
    std::vector<std::string> description;
    std::string synopsis;

public:
    void add_command(const std::string &name, const std::string &description);
    void add_description(const std::string &line);
    void add_example(const std::string &description, const std::string &command);
    void add_name_version();
    void add_option(const std::string &opt_short, const std::string &opt_long, const std::string &description);
    void add_synopsis(const std::string &line);

    void print();
};

void HelpMessages::add_command(const std::string &name, const std::string &description)
{
    this->commands.push_back({ description, name });
}

void HelpMessages::add_description(const std::string &line)
{
    this->description.push_back(line);
}

void HelpMessages::add_example(const std::string &description, const std::string &command)
{
    this->examples.push_back({ command, description });
}

void HelpMessages::add_name_version()
{
    this->print_metadata = true;
}

void HelpMessages::add_option(const std::string &opt_short, const std::string &opt_long, const std::string &description)
{
    this->options.push_back({ description, opt_long, opt_short });
}

void HelpMessages::add_synopsis(const std::string &line)
{
    this->synopsis = line;
}

void HelpMessages::print_commands()
{
    if (this->commands.empty()) {
        return;
    }

    fmt::print(fg(white), "Commands:\n");

    for (const auto &it: this->commands) {
        fmt::print(fg(blue), "{}{}\n", ws_2, it.name);
        fmt::print("{} -> {}\n", ws_4, it.description);
    }

    fmt::print("\n{}Try gpt ", ws_2);
    fmt::print(fg(blue), "<subcommand>");
    fmt::print(" [-h | --help] for subcommand specific help.\n\n");
}

void HelpMessages::print_description()
{
    if (this->description.empty()) {
        return;
    }

    fmt::print(fg(white), "Description:\n");

    for (const auto &it: this->description) {
        fmt::print("{}{}\n", ws_2, it);
    }

    fmt::print("\n");
}

void HelpMessages::print_examples()
{
    if (this->examples.empty()) {
        return;
    }

    fmt::print(fg(white), "Examples:\n");

    for (const auto &it: this->examples) {
        fmt::print("{}{}:\n", ws_2, it.description);
        print_bash_block(it.command);
    }

    fmt::print("\n");
}

void HelpMessages::print_name_version()
{
    if (not this->print_metadata) {
        return;
    }

    const std::string name = std::string(PROJECT_NAME);
    const std::string version = std::string(PROJECT_VERSION);

    fmt::print(fg(white), "Name:\n");
    fmt::print("{}", ws_2);
    fmt::print(fmt::emphasis::underline, "{} v{}\n\n", name, version);
}

void HelpMessages::print_options()
{
    if (this->options.empty()) {
        return;
    }

    fmt::print(fg(white), "Options:\n");

    for (const auto &it: this->options) {
        fmt::print(fmt::emphasis::faint, "{}{}, {}\n", ws_2, it.opt_short, it.opt_long);
        fmt::print("{} -> {}\n", ws_4, it.description);
    }

    fmt::print("\n");
}

void HelpMessages::print_synopsis()
{
    if (this->synopsis.empty()) {
        return;
    }

    fmt::print(fg(white), "Synopsis:\n");
    fmt::print("{}gpt {}\n\n", ws_2, this->synopsis);
}

void HelpMessages::print()
{
    this->print_name_version();
    this->print_description();
    this->print_synopsis();
    this->print_options();
    this->print_commands();
    this->print_examples();
}

} // namespace

namespace cli {

void exit_on_failure()
{
    fmt::print(stderr, "Try running with -h or --help for more information\n");
    exit(EXIT_FAILURE);
}

void help_root_messages()
{
    HelpMessages help;
    help.add_name_version();
    help.add_description("A command line program for interactively querying OpenAI via the OpenAI API.");
    help.add_description("See \033[4mhttps://github.com/dsw7/GPTifier\033[0m for more information.");
    help.add_synopsis("[-v | --version] [-h | --help] <command> [<args>]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-v", "--version", "Print version and exit");
    help.add_command("run", "Run a query against an appropriate model");
    help.add_command("short", "Run a query against an appropriate model but with no threading and limited verbosity");
    help.add_command("models", "List available OpenAI models");
    help.add_command("embed", "Get embedding representing a block of text");
    help.add_command("files", "Manage files uploaded to OpenAI");
    help.add_command("fine-tune", "Manage fine tuning operations");
    help.add_command("costs", "Get OpenAI usage details");
    help.add_command("edit", "Edit one or more files according to a prompt");
    help.print();
}

void help_command_run()
{
    HelpMessages help;
    help.add_description("Create a chat completion.");
    help.add_synopsis(
        "run [-h | --help] [-m <model-name> | --model=<model-name>]\n  "
        "[-u | --no-interactive-export] [-o <file> | --file=<file>]\n  "
        "[-p <prompt> | --prompt=<prompt>] [-r <filename> | --read-from-file=<filename>]\n  "
        "[-t <temp> | --temperature=<temperature>] [-s | --store-completion]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-m <model-name>", "--model=<model-name>", "Specify a valid chat model");
    help.add_option("-u", "--no-interactive-export", "Disable [y/n] prompt that asks whether to export results");
    help.add_option("-o <file>", "--file=<file>", "Export results to a JSON file");
    help.add_option("-p <prompt>", "--prompt=<prompt>", "Provide prompt via command line");
    help.add_option("-r <filename>", "--read-from-file=<filename>", "Read prompt from a custom file");
    help.add_option("-t <temp>", "--temperature=<temperature>", "Provide a sampling temperature between 0 and 2");
    help.add_option("-s", "--store-completion", "Store results of completion on OpenAI servers");
    help.add_example("Run an interactive session", "gpt run");
    help.add_example("Run a query non-interactively and export results", "gpt run --prompt=\"What is 3 + 5\" --file=\"/tmp/results.json\"");
    help.print();
}

void help_command_short()
{
    HelpMessages help;
    help.add_description("Create a chat completion but without threading or verbosity.");
    help.add_synopsis("short [-h | --help] [-j | --json] [-t <temp> | --temperature=<temp>]\n  "
                      "[-s | --store-completion] PROMPT");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.add_option("-t <temp>", "--temperature=<temperature>", "Provide a sampling temperature between 0 and 2");
    help.add_option("-s", "--store-completion", "Store results of completion on OpenAI servers");
    help.add_example("Create a chat completion", "gpt short \"What is 2 + 2?\"");
    help.print();
}

void help_command_models()
{
    HelpMessages help;
    help.add_description("List available OpenAI or user models.");
    help.add_synopsis("models [-h | --help] [-j | --json] [-u | --user]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.add_option("-u", "--user", "Print user models if they exist");
    help.print();
}

void help_command_embed()
{
    HelpMessages help;
    help.add_description("Get embedding representing a block of text.");
    help.add_synopsis(
        "embed [-h | --help] [-m <model> | --model=<model>]\n  "
        "[-i <text> | --input=<text>] [-o <filename> | --output-file=<filename>]\n  "
        "[-r <filename> | --read-from-file=<filename>]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-m <model-name>", "--model=<model-name>", "Specify a valid embedding model");
    help.add_option("-i <text>", "--input=<text>", "Input text to embed");
    help.add_option("-r <filename>", "--read-from-file=<filename>", "Read input text to embed from a file");
    help.add_option("-o <filename>", "--output-file=<filename>", "Export embedding to <filename>");
    help.print();
}

void help_command_files()
{
    HelpMessages help;
    help.add_description("Manage files uploaded to OpenAI.");
    help.add_synopsis("files [-h | --help] (list | delete)");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_command("list", "List uploaded files");
    help.add_command("delete", "Delete an uploaded file");
    help.print();
}

void help_command_files_list()
{
    HelpMessages help;
    help.add_description("List uploaded files.");
    help.add_synopsis("files list [-h | --help] [-j | --json]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.print();
}

void help_command_files_delete()
{
    HelpMessages help;
    help.add_description("Delete an uploaded file.");
    help.add_synopsis("files delete [-h | --help] <file-id...>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.print();
}

void help_command_fine_tune()
{
    HelpMessages help;
    help.add_description("Manage fine tuning operations.");
    help.add_synopsis("fine-tune [-h | --help] (upload-file | create-job | delete-model | list-jobs)");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_command("upload-file", "Upload a fine-tuning file");
    help.add_command("create-job", "Create a fine-tuning job");
    help.add_command("delete-model", "Delete a fine-tuned model");
    help.add_command("list-jobs", "List fine-tuning jobs");
    help.print();
}

void help_command_fine_tune_upload_file()
{
    HelpMessages help;
    help.add_description("Upload a fine-tuning file.");
    help.add_synopsis("fine-tune upload-file [-h | --help] <file>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.print();
}

void help_command_fine_tune_create_job()
{
    HelpMessages help;
    help.add_description("Create a fine-tuning job.");
    help.add_synopsis(
        "fine-tune create-job [-h | --help] "
        "-f <file-id> | --file-id <file-id> -m <model> | --model <model>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-f", "--file-id", "The ID of an uploaded file that contains the training data");
    help.add_option("-m", "--model", "The name of the model to fine-tune");
    help.print();
}

void help_command_fine_tune_delete_model()
{
    HelpMessages help;
    help.add_description("Delete a fine-tuned model.");
    help.add_synopsis("fine-tune delete-model [-h | --help] <model>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.print();
}

void help_command_fine_tune_list_jobs()
{
    HelpMessages help;
    help.add_description("List fine-tuning jobs.");
    help.add_synopsis("fine-tune list-jobs [-h | --help] [-j | --json] -l <limit> | --limit <limit>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.add_option("-l", "--limit", "Number of fine-tuning jobs to show");
    help.print();
}

void help_command_costs()
{
    HelpMessages help;
    help.add_description("Get OpenAI usage details.");
    help.add_synopsis("costs [-h | --help] [-j | --json] -d <days> | --days <days>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.add_option("-d", "--days", "Select number of days to go back");
    help.print();
}

void help_command_chats()
{
    HelpMessages help;
    help.add_description("Manage chat completions uploaded to OpenAI.");
    help.add_synopsis("chats [-h | --help] (list | delete)");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_command("list", "List uploaded chat completions");
    help.add_command("delete", "Delete an uploaded chat completion");
    help.print();
}

void help_command_chats_list()
{
    HelpMessages help;
    help.add_description("List uploaded chat completions.");
    help.add_synopsis("chats list [-h | --help] [-j | --json] -l <limit> | --limit <limit>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.add_option("-l", "--limit", "Number of chat completions to show");
    help.print();
}

void help_command_chats_delete()
{
    HelpMessages help;
    help.add_description("Delete an uploaded chat completion.");
    help.add_synopsis("chats delete [-h | --help] <chat-completion-id...>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.print();
}

void help_command_edit()
{
    HelpMessages help;
    help.add_description("Edit one or more files according to a prompt.");
    help.add_synopsis("edit [-h | --help] [-d | --debug]\n  "
                      "[-m <model> | --model <model>] [-o <filename> | --output <filename>]\n  "
                      "[-p <filename> | --prompt <filename>] <input-file>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-d", "--debug", "Print raw prompt and completion. Will not edit file");
    help.add_option("-m <model-name>", "--model=<model-name>", "Specify a valid chat model");
    help.add_option("-o <filename>", "--output=<filename>", "Specify where to export edited code");
    help.add_option("-p <filename>", "--prompt=<filename>", "Specify instructions to apply to input file");
    help.add_example("Edit a file and print changes to stdout", "gpt edit foo.cpp -p prompt.txt");
    help.add_example("Edit a file and write changes to new file", "gpt edit foo.cpp -p prompt.txt -o bar.cpp");
    help.add_example("Overwrite an existing file with edits", "gpt edit foo.cpp -o foo.cpp -p prompt.txt");
    help.print();
}

} // namespace cli
