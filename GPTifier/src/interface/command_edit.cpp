#include "interface/command_edit.hpp"

#include "interface/help_messages.hpp"

#include <cstring>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Params {
    std::optional<std::string> prompt_file = std::nullopt;
    std::vector<std::string> files;
};

void read_cli(int argc, char **argv, Params &params)
{
    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int opt = getopt_long(argc, argv, "h", long_options, &option_index);

        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'h':
                cli::help_command_edit();
                exit(EXIT_SUCCESS);
            default:
                cli::exit_on_failure();
        }
    }

    bool prompt_set = false;

    for (int i = optind; i < argc; i++) {
        if (strcmp("edit", argv[i]) != 0) {
            if (prompt_set) {
                params.files.push_back(argv[i]);
            } else {
                params.prompt_file = argv[i];
                prompt_set = true;
            }
            break;
        }
    }
}

} // namespace

void command_edit(int argc, char **argv)
{
    Params params;
    read_cli(argc, argv, params);

    if (not params.prompt_file) {
        throw std::runtime_error("No prompt file provided. Cannot proceed");
    }

    std::cout << params.files[0] << '\n';
}
