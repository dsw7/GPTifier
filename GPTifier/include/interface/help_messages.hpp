#pragma once

#include <string>
#include <vector>

namespace help {

void exit_on_failure();

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

} // namespace help
