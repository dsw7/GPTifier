#include "cli.hpp"

#include "help_messages.hpp"

#include <getopt.h>
#include <iostream>

namespace {

void exit_on_failure()
{
    std::cerr << "Try running with -h or --help for more information\n";
    exit(EXIT_FAILURE);
}

} // namespace

namespace cli {

bool get_opts_models(int argc, char **argv)
{
    bool print_raw_json = false;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' }, { 0, 0, 0, 0 },
            { "raw", no_argument, 0, 'r' }, { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hr", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_models();
                exit(EXIT_SUCCESS);
            case 'r':
                print_raw_json = true;
                break;
            default:
                exit_on_failure();
        }
    }

    return print_raw_json;
}

std::optional<std::string> get_opts_short(int argc, char **argv)
{
    std::optional<std::string> prompt = std::nullopt;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' }, { "prompt", required_argument, 0, 'p' }, { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hp:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_short();
                exit(EXIT_SUCCESS);
            case 'p':
                prompt = optarg;
                break;
            default:
                exit_on_failure();
        }
    }

    return prompt;
}

ParamsEmbedding get_opts_embed(int argc, char **argv)
{
    ParamsEmbedding params;

    while (true) {
        static struct option long_options[] = { { "help", no_argument, 0, 'h' },
            { "model", required_argument, 0, 'm' },
            { "input", required_argument, 0, 'i' },
            { "read-from-file", required_argument, 0, 'r' },
            { 0, 0, 0, 0 } };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hm:i:r:", long_options, &option_index);

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
            case 'r':
                params.input_file = optarg;
                break;
            default:
                exit_on_failure();
        }
    }

    return params;
}

ParamsRun get_opts_run(int argc, char **argv)
{
    ParamsRun params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "no-interactive-export", no_argument, 0, 'u' },
            { "dump", required_argument, 0, 'd' },
            { "model", required_argument, 0, 'm' },
            { "prompt", required_argument, 0, 'p' },
            { "read-from-file", required_argument, 0, 'r' },
            { "temperature", required_argument, 0, 't' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hud:m:p:r:t:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_run();
                exit(EXIT_SUCCESS);
            case 'u':
                params.enable_export = false;
                break;
            case 'd':
                params.json_dump_file = optarg;
                break;
            case 'p':
                params.prompt = optarg;
                break;
            case 't':
                params.temperature = optarg;
                break;
            case 'r':
                params.prompt_file = optarg;
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
            { "raw", no_argument, 0, 'r' },
            { "limit", required_argument, 0, 'l' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hrl:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_fine_tune_list_jobs();
                exit(EXIT_SUCCESS);
            case 'r':
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
