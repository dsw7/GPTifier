#include "command_img.hpp"

#include "help_messages.hpp"
#include "images.hpp"
#include "utils.hpp"

#include <ctime>
#include <fmt/core.h>
#include <getopt.h>
#include <optional>
#include <stdexcept>

namespace {

void help_img()
{
    help::HelpMessages help;
    help.add_description("Generate an image from a prompt");
    help.add_synopsis("img [OPTIONS] PROMPT-FILE");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-q", "--hd", "Request high definition image (default is standard)");
    help.add_option("-v", "--vivid", "Request hyper-realistic / dramatic image (default is natural)");
    help.add_option("-o <filename>", "--output=<filename>", "Specify where to export image (defaults to timestamp returned from OpenAI API)");
    help.print();
}

struct Parameters {
    std::string model = "dall-e-3";
    std::string quality = "standard";
    std::string style = "natural";
    std::optional<std::string> prompt_file;
    std::optional<std::string> output_file;
};

Parameters read_cli(int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "hd", no_argument, 0, 'q' },
            { "vivid", no_argument, 0, 'v' },
            { "output", required_argument, 0, 'o' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hqvo:", long_options, &option_index);

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
            case 'o':
                params.output_file = optarg;
                break;
            default:
                help::exit_on_failure();
        }
    };

    for (int i = optind; i < argc; i++) {
        if (strcmp("img", argv[i]) != 0) {
            params.prompt_file = argv[i];
            break;
        }
    }

    return params;
}

std::string filename_from_created(const std::time_t &timestamp)
{
    const std::tm *datetime = std::gmtime(&timestamp);
    char buffer[80];

    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", datetime);
    return fmt::format("{}.png", buffer);
}

std::string base64_decode(const std::string &str_encoded)
{
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) {
        T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    }

    int val = 0;
    int valb = -8;
    std::string str_decoded;

    for (unsigned char c: str_encoded) {
        if (T[c] == -1) {
            break; // i.e. non-base64 character is found
        }
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            str_decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return str_decoded;
}

} // namespace

namespace commands {

void command_img(int argc, char **argv)
{
    const Parameters params = read_cli(argc, argv);

    if (not params.prompt_file) {
        throw std::runtime_error("No prompt file provided. Cannot proceed");
    }

    const std::string prompt = utils::read_from_file(params.prompt_file.value());
    const serialization::Image image = serialization::create_image(params.model, prompt, params.quality, params.style);

    std::string output_file;

    if (params.output_file) {
        output_file = params.output_file.value();
    } else {
        output_file = filename_from_created(image.created);
    }

    const std::string b64_decoded = base64_decode(image.b64_json);
    utils::write_to_png(output_file, b64_decoded);

    fmt::print("Input text tokens: {}\n", image.input_tokens_text);
    fmt::print("Input image tokens: {}\n", image.input_tokens_image);
    fmt::print("Output tokens: {}\n", image.output_tokens);
    fmt::print("Exported image to {}\n", output_file);
}

} // namespace commands
