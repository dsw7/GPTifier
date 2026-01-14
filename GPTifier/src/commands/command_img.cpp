#include "command_img.hpp"

#include "images.hpp"
#include "utils.hpp"

#include <ctime>
#include <fmt/core.h>
#include <getopt.h>
#include <json.hpp>
#include <optional>
#include <stdexcept>

namespace {

void help_img()
{
    const std::string messages = R"(Generate an image from a prompt. Command currently defaults to using the
DALL-E 3 model for image generation.

Usage:
  gpt img [OPTIONS] PROMPT-FILE

Options:
  -h, --help   Print help information and exit
  -q, --hd     Request high definition image (default is standard definition for cost savings)
  -v, --vivid  Request hyper-realistic / dramatic image (default is natural)
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    std::string model = "dall-e-3";
    std::string quality = "standard";
    std::string style = "natural";
    std::optional<std::string> prompt_file;
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
                utils::exit_on_failure();
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
    return buffer;
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

void export_image(const serialization::Image &image, const std::string &filename_png)
{
    const std::string b64_decoded = base64_decode(image.b64_json);
    utils::write_to_png(filename_png, b64_decoded);
    fmt::print("Exported image to {}\n", filename_png);
}

void export_image_metadata(const nlohmann::json &metadata, const std::string &filename)
{
    const std::string filename_json = fmt::format("{}.json", filename);
    utils::write_to_file(filename_json, metadata.dump(4));
    fmt::print("Exported image metadata to {}\n", filename_json);
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

    const std::string filename = filename_from_created(image.created);
    const std::string filename_png = fmt::format("{}.png", filename);

    export_image(image, filename_png);

    nlohmann::json metadata = {
        { "filename", filename_png },
        { "model", params.model },
        { "prompt", prompt },
        { "quality", params.quality },
        { "style", params.style },
    };

    if (image.revised_prompt) {
        metadata["revised_prompt"] = image.revised_prompt.value();
    }

    export_image_metadata(metadata, filename);
}

} // namespace commands
