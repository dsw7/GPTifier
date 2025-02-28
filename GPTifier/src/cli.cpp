#include "cli.hpp"

#include "help_messages.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <string.h>

namespace {

void exit_on_failure()
{
    fmt::print(stderr, "Try running with -h or --help for more information\n");
    exit(EXIT_FAILURE);
}

} // namespace

namespace cli {

ParamsRun get_opts_run(int argc, char **argv)
{
    ParamsRun params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "no-interactive-export", no_argument, 0, 'u' },
            { "store-completion", no_argument, 0, 's' },
            { "file", required_argument, 0, 'o' },
            { "model", required_argument, 0, 'm' },
            { "prompt", required_argument, 0, 'p' },
            { "read-from-file", required_argument, 0, 'r' },
            { "temperature", required_argument, 0, 't' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "huso:m:p:r:t:", long_options, &option_index);

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
            case 's':
                params.store_completion = true;
                break;
            case 'o':
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

    params.sanitize();
    return params;
}

ParamsShort get_opts_short(int argc, char **argv)
{
    ParamsShort params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "temperature", required_argument, 0, 't' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hjt:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_short();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 't':
                params.temperature = optarg;
                break;
            default:
                exit_on_failure();
        }
    }

    for (int i = optind; i < argc; i++) {
        if (strcmp("short", argv[i]) != 0) {
            params.prompt = argv[i];
            break;
        }
    }

    params.sanitize();
    return params;
}

ParamsModels get_opts_models(int argc, char **argv)
{
    ParamsModels params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "user", no_argument, 0, 'u' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hju", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_models();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 'u':
                params.print_user_models = true;
                break;
            default:
                exit_on_failure();
        }
    }

    return params;
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

ParamsCosts get_opts_get_costs(int argc, char **argv)
{
    ParamsCosts params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "days", required_argument, 0, 'd' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hjd:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_command_costs();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 'd':
                params.days = optarg;
                break;
            default:
                exit_on_failure();
        }
    };

    params.sanitize();
    return params;
}

ParamsGetChatCompletions get_opts_get_chat_completions(int argc, char **argv)
{
    ParamsGetChatCompletions params;

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
                help_command_chats_list();
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

    params.sanitize();
    return params;
}

} // namespace cli
