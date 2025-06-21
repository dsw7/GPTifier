#include "command_fine_tune.hpp"

#include "api_openai_user.hpp"
#include "files.hpp"
#include "fine_tuning.hpp"
#include "help_messages.hpp"
#include "models.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <getopt.h>
#include <optional>
#include <stdexcept>
#include <string>

namespace {

void help_fine_tune()
{
    help::HelpMessages help;
    help.add_description("Manage fine tuning operations.");
    help.add_synopsis("fine-tune [OPTIONS]");
    help.add_synopsis("fine-tune upload-file [OPTIONS] FILE");
    help.add_synopsis("fine-tune create-job [OPTIONS]");
    help.add_synopsis("fine-tune delete-model [OPTIONS] MODEL");
    help.add_synopsis("fine-tune list-jobs [OPTIONS]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_command("upload-file", "Upload a fine-tuning file");
    help.add_command("create-job", "Create a fine-tuning job");
    help.add_command("delete-model", "Delete a fine-tuned model");
    help.add_command("list-jobs", "List fine-tuning jobs");
    help.print();
}

void help_fine_tune_upload_file()
{
    help::HelpMessages help;
    help.add_description("Upload a fine-tuning file.");
    help.add_synopsis("fine-tune upload-file [OPTIONS] FILE");
    help.add_option("-h", "--help", "Print help information and exit");
    help.print();
}

void help_fine_tune_create_job()
{
    help::HelpMessages help;
    help.add_description("Create a fine-tuning job.");
    help.add_synopsis("fine-tune create-job [OPTIONS]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-f", "--file-id", "The ID of an uploaded file that contains the training data");
    help.add_option("-m", "--model", "The name of the model to fine-tune");
    help.print();
}

void help_fine_tune_delete_model()
{
    help::HelpMessages help;
    help.add_description("Delete a fine-tuned model.");
    help.add_synopsis("fine-tune delete-model [OPTIONS] MODEL");
    help.add_option("-h", "--help", "Print help information and exit");
    help.print();
}

void help_fine_tune_list_jobs()
{
    help::HelpMessages help;
    help.add_description("List fine-tuning jobs.");
    help.add_synopsis("fine-tune list-jobs [OPTIONS]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.add_option("-l", "--limit", "Number of fine-tuning jobs to show");
    help.print();
}

// Upload fine tuning file ----------------------------------------------------------------------------------

void upload_fine_tuning_file(int argc, char **argv)
{
    if (argc == 3) {
        help_fine_tune_upload_file();
        return;
    }

    const std::string opt_or_filename = argv[3];

    if (opt_or_filename == "-h" or opt_or_filename == "--help") {
        help_fine_tune_upload_file();
        return;
    }

    const std::string id = serialization::upload_file(opt_or_filename);
    fmt::print("Success!\nUploaded file: {}\nWith ID: {}\n", opt_or_filename, id);
}

// Create fine tuning job -----------------------------------------------------------------------------------

void create_fine_tuning_job(int argc, char **argv)
{
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> training_file = std::nullopt;

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
                help_fine_tune_create_job();
                exit(EXIT_SUCCESS);
            case 'f':
                training_file = optarg;
                break;
            case 'm':
                model = optarg;
                break;
            default:
                help::exit_on_failure();
        }
    };

    utils::separator();

    if (training_file) {
        fmt::print("Training using file with ID: {}\n", training_file.value());
    } else {
        throw std::runtime_error("No training file ID provided");
    }

    if (model) {
        fmt::print("Training model: {}\n", model.value());
    } else {
        throw std::runtime_error("No model provided");
    }

    utils::separator();

    const std::string id = serialization::create_fine_tuning_job(model.value(), training_file.value());
    fmt::print("Deployed fine tuning job with ID: {}\n", id);
}

// Delete fine tuned model-----------------------------------------------------------------------------------

void delete_fine_tuned_model(int argc, char **argv)
{
    if (argc == 3) {
        help_fine_tune_delete_model();
        return;
    }

    const std::string opt_or_model_id = argv[3];

    if (opt_or_model_id == "-h" or opt_or_model_id == "--help") {
        help_fine_tune_delete_model();
        return;
    }

    if (serialization::delete_model(opt_or_model_id)) {
        fmt::print("Success!\nDeleted model with ID: {}\n", opt_or_model_id);
    } else {
        fmt::print("Warning!\nDid not delete model with ID: {}\n", opt_or_model_id);
    }
}

// Print fine tuning jobs -----------------------------------------------------------------------------------

void print_fine_tuning_jobs(const serialization::FineTuningJobs &jobs)
{
    utils::separator();
    fmt::print("{:<40}{:<30}{:<30}{}\n", "Job ID", "Created at", "Estimated finish", "Finished at");
    utils::separator();

    for (const auto &job: jobs.jobs) {
        const std::string dt_created_at = utils::datetime_from_unix_timestamp(job.created_at);
        fmt::print("{:<40}{:<30}{:<30}{}\n", job.id, dt_created_at, job.estimated_finish, job.finished_at);
    }

    utils::separator();
}

void list_fine_tuning_jobs(int argc, char **argv)
{
    bool print_raw_json = false;
    std::optional<std::string> limit = std::nullopt;

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
                help_fine_tune_list_jobs();
                exit(EXIT_SUCCESS);
            case 'j':
                print_raw_json = true;
                break;
            case 'l':
                limit = optarg;
                break;
            default:
                help::exit_on_failure();
        }
    };

    serialization::FineTuningJobs jobs = serialization::get_fine_tuning_jobs(limit.value_or("20"));

    if (print_raw_json) {
        fmt::print("{}\n", jobs.raw_response);
        return;
    }

    if (not limit) {
        utils::separator();
        fmt::print("> No limit passed with --limit flag. Will use OpenAI's default retrieval limit of 20 listings\n");
    }

    std::sort(jobs.jobs.begin(), jobs.jobs.end());
    print_fine_tuning_jobs(jobs);
}

} // namespace

namespace commands {

void command_fine_tune(int argc, char **argv)
{
    if (argc == 2) {
        help_fine_tune();
        exit(EXIT_FAILURE);
    }

    const std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        help_fine_tune();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "upload-file") {
        upload_fine_tuning_file(argc, argv);
    } else if (subcommand == "create-job") {
        create_fine_tuning_job(argc, argv);
    } else if (subcommand == "delete-model") {
        delete_fine_tuned_model(argc, argv);
    } else if (subcommand == "list-jobs") {
        list_fine_tuning_jobs(argc, argv);
    } else {
        help_fine_tune();
        exit(EXIT_FAILURE);
    }
}

} // namespace commands
