#include "command_fine_tune.hpp"

#include "api_openai_user.hpp"
#include "files.hpp"
#include "fine_tuning.hpp"
#include "models.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <getopt.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

void help_fine_tune_()
{
    const std::string messages = R"(Manage all fine-tuning operations.

Usage:
  gpt fine-tune [OPTIONS] COMMAND [ARGS]...

Options:
  -h, --help  Print help information and exit

Commands:
  upload-file   Upload a fine-tuning file
  create-job    Create a fine-tuning job
  delete-model  Delete a fine-tuned model
  list-jobs     List fine-tuning jobs
)";

    fmt::print("{}\n", messages);
}

void help_fine_tune_upload_file_()
{
    const std::string messages = R"(Upload a fine-tuning file. The file must be in JSONL format.

Usage:
  gpt fine-tune upload-file [OPTIONS] FILE

Options:
  -h, --help  Print help information and exit
)";

    fmt::print("{}\n", messages);
}

void help_fine_tune_create_job_()
{
    const std::string messages = R"(Create a fine-tuning job. Command will fine-tune a model named
MODEL. Note that this command assumes that a fine-tuning file
has already been uploaded to OpenAI servers.

Usage:
  gpt fine-tune create-job [OPTIONS] FILE-ID MODEL

Options:
  -h, --help  Print help information and exit

The file ID can be obtained by running the 'gpt files list' command
)";

    fmt::print("{}\n", messages);
}

void help_fine_tune_delete_model_()
{
    const std::string messages = R"(Delete a fine-tuned model.

Usage:
  gpt fine-tune delete-model [OPTIONS] MODEL-ID

Options:
  -h, --help  Print help information and exit

The model ID can be obtained by running the 'gpt models --user' command
)";

    fmt::print("{}\n", messages);
}

void help_fine_tune_list_jobs_()
{
    const std::string messages = R"(List fine-tuning jobs. This command can also be used to track the progress
of a fine-tuning job.

Usage:
  gpt fine-tune list-jobs [OPTIONS]

Options:
  -h, --help         Print help information and exit
  -j, --json         Print raw JSON response from OpenAI
  -l, --limit=LIMIT  Show LIMIT number of fine-tuning jobs. LIMIT will
                     be clamped to list between 1 and 100 jobs
)";

    fmt::print("{}\n", messages);
}

// Upload fine tuning file ----------------------------------------------------------------------------------

void upload_fine_tuning_file_(const int argc, char **argv)
{
    if (argc == 3) {
        throw std::runtime_error("A fine tuning file needs to be provided");
    }

    const std::string opt_or_filename = argv[3];

    if (opt_or_filename == "-h" or opt_or_filename == "--help") {
        help_fine_tune_upload_file_();
        return;
    }

    if (opt_or_filename.empty()) {
        throw std::runtime_error("Filename is empty");
    }

    const std::string id = serialization::upload_file(opt_or_filename);
    fmt::print("Success!\nUploaded file: {}\nWith ID: {}\n", opt_or_filename, id);
}

// Create fine tuning job -----------------------------------------------------------------------------------

void create_fine_tuning_job_(const int argc, char **argv)
{
    if (argc < 4) {
        throw std::runtime_error("A fine tuning file ID and model needs to be provided");
    } else if (argc == 4) {
        const std::string opt = argv[3];

        if (opt == "-h" or opt == "--help") {
            help_fine_tune_create_job_();
            exit(EXIT_SUCCESS);
        } else {
            fmt::print(stderr, "Unknown option: '{}'\n", opt);
            exit(EXIT_FAILURE);
        }
    }

    const std::string training_file_id = argv[3];
    const std::string model = argv[4];

    if (training_file_id.empty()) {
        throw std::runtime_error("Training file ID argument is empty");
    }

    if (model.empty()) {
        throw std::runtime_error("Model argument is empty");
    }

    fmt::print("Training using file with ID: {}\n", training_file_id);
    fmt::print("Training model: {}\n", model);

    const std::string id = serialization::create_fine_tuning_job(model, training_file_id);
    fmt::print("Deployed fine tuning job with ID: {}\n", id);
}

// Delete fine tuned model-----------------------------------------------------------------------------------

void delete_fine_tuned_model_(const int argc, char **argv)
{
    if (argc == 3) {
        throw std::runtime_error("A model ID needs to be provided");
    }

    const std::string opt_or_model_id = argv[3];

    if (opt_or_model_id == "-h" or opt_or_model_id == "--help") {
        help_fine_tune_delete_model_();
        return;
    }

    if (opt_or_model_id.empty()) {
        throw std::runtime_error("Model ID argument is empty");
    }

    const bool was_deleted = serialization::delete_model(opt_or_model_id);

    if (was_deleted) {
        fmt::print("Success!\nDeleted model with ID: {}\n", opt_or_model_id);
    } else {
        fmt::print("Warning!\nDid not delete model with ID: {}\n", opt_or_model_id);
    }
}

// Print fine tuning jobs -----------------------------------------------------------------------------------

using serialization::FineTuningJobs;
using VecFineTuningJobs = std::vector<serialization::FineTuningJob>;

void print_fine_tuning_jobs_(const VecFineTuningJobs &jobs)
{
    fmt::print("{:<40}{:<30}{:<30}{}\n", "Job ID", "Created at", "Estimated finish", "Finished at");

    for (const auto &job: jobs) {
        fmt::print("{:<40}{:<30}{:<30}{}\n", job.id, job.created_at_dt_str, job.estimated_finish, job.finished_at);
    }
}

void list_fine_tuning_jobs_(const int argc, char **argv)
{
    bool print_raw_json = false;
    std::optional<std::string> limit;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "limit", required_argument, 0, 'l' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hjl:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_fine_tune_list_jobs_();
                exit(EXIT_SUCCESS);
            case 'j':
                print_raw_json = true;
                break;
            case 'l':
                limit = optarg;
                break;
            default:
                utils::exit_on_failure();
        }
    };

    if (limit) {
        if (limit.value().empty()) {
            throw std::runtime_error("Limit is empty");
        }
    }

    const FineTuningJobs response = serialization::get_fine_tuning_jobs(
        utils::string_to_int(limit.value_or("20")));

    if (print_raw_json) {
        fmt::print("{}\n", response.raw_response);
        return;
    }

    if (not limit) {
        fmt::print("No limit passed with --limit flag. Will use OpenAI's default retrieval limit of 20 listings\n\n");
    }

    VecFineTuningJobs jobs = std::move(response.jobs);
    std::sort(jobs.begin(), jobs.end());

    print_fine_tuning_jobs_(jobs);
}

} // namespace

namespace commands {

void command_fine_tune(const int argc, char **argv)
{
    if (argc == 2) {
        help_fine_tune_();
        exit(EXIT_FAILURE);
    }

    const std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        help_fine_tune_();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "upload-file") {
        upload_fine_tuning_file_(argc, argv);
    } else if (subcommand == "create-job") {
        create_fine_tuning_job_(argc, argv);
    } else if (subcommand == "delete-model") {
        delete_fine_tuned_model_(argc, argv);
    } else if (subcommand == "list-jobs") {
        list_fine_tuning_jobs_(argc, argv);
    } else {
        help_fine_tune_();
        exit(EXIT_FAILURE);
    }
}

} // namespace commands
