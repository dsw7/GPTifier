#include "command_img.hpp"

#include "help_messages.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <getopt.h>
#include <optional>
#include <stdexcept>

namespace {

void help_img()
{
    help::HelpMessages help;
    help.add_description("Generate an image from a prompt");
    help.add_synopsis("img [OPTIONS] </path/to/prompt/file>");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-q", "--hd", "Request high definition image (default is standard)");
    help.add_option("-v", "--vivid", "Request hyper-realistic / dramatic image (default is natural)");
    help.print();
}

struct Parameters {
    std::string model = "dall-e-3";
    std::string quality = "standard";
    std::string size = "1024x1024";
    std::string style = "natural";
    std::optional<std::filesystem::path> prompt_file;
};

Parameters read_cli(int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "hd", no_argument, 0, 'q' },
            { "vivid", no_argument, 0, 'v' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hqv", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_img();
                exit(EXIT_SUCCESS);
            case 'q':
                params.quality = "hd";
                break;
            case 'v':
                params.style = "vivid";
                break;
            default:
                help::exit_on_failure();
        }
    };

    for (int i = optind; i < argc; i++) {
        params.prompt_file = argv[i];
    }

    if (not params.prompt_file) {
        throw std::runtime_error("No prompt file provided. Cannot proceed");
    }

    return params;
}

} // namespace

namespace commands {

void command_img(int argc, char **argv)
{
    if (argc == 2) {
        help_img();
        exit(EXIT_FAILURE);
    }

    const Parameters params = read_cli(argc, argv);
    fmt::print("{}\n", params.style);
}

} // namespace commands
