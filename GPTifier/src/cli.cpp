#include "cli.hpp"

#include "help_messages.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <string.h>

namespace cli {

void exit_on_failure()
{
    fmt::print(stderr, "Try running with -h or --help for more information\n");
    exit(EXIT_FAILURE);
}

ParamsEmbedding get_opts_embed(int argc, char **argv)
{
    ParamsEmbedding params;

    while (true) {
        static struct option long_options[] = { { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "input", required_argument, 0, 'i' },
            { "output-file", required_argument, 0, 'o' },
            { "read-from-file", required_argument, 0, 'r' },
            { 0, 0, 0, 0 } };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hm:i:o:r:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_embed();
                exit(EXIT_SUCCESS);
            case 'm':
                params.model = optarg;
                break;
            case 'i':
                params.input = optarg;
                break;
            case 'o':
                params.output_file = optarg;
                break;
            case 'r':
                params.input_file = optarg;
                break;
            default:
                exit_on_failure();
        }
    }

    return params;
}

bool get_opts_files_list(int argc, char **argv)
{
    bool print_raw_json = false;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hj", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_files_list();
                exit(EXIT_SUCCESS);
            case 'j':
                print_raw_json = true;
                break;
            default:
                exit_on_failure();
        }
    }

    return print_raw_json;
}

ParamsFineTune get_opts_create_fine_tuning_job(int argc, char **argv)
{
    ParamsFineTune params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "file-id", required_argument, 0, 'f' },
            { "model", required_argument, 0, 'm' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hf:m:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_fine_tune_create_job();
                exit(EXIT_SUCCESS);
            case 'f':
                params.training_file = optarg;
                break;
            case 'm':
                params.model = optarg;
                break;
            default:
                exit_on_failure();
        }
    };

    return params;
}

ParamsGetFineTuningJobs get_opts_get_fine_tuning_jobs(int argc, char **argv)
{
    ParamsGetFineTuningJobs params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "limit", required_argument, 0, 'l' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hjl:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_fine_tune_list_jobs();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 'l':
                params.limit = optarg;
                break;
            default:
                exit_on_failure();
        }
    };

    return params;
}

} // namespace cli
