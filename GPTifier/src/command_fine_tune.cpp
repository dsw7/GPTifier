#include "command_fine_tune.hpp"

#include "cli.hpp"
#include "help_messages.hpp"
#include "models.hpp"
#include "networking/api_openai_user.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "serialization/files.hpp"
#include "serialization/fine_tuning.hpp"
#include "serialization/models.hpp"
#include "utils.hpp"
#include "validation.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

// Upload fine tuning file ----------------------------------------------------------------------------------

void upload_fine_tuning_file(int argc, char **argv)
{
    if (argc < 4) {
        cli::help_command_fine_tune_upload_file();
        return;
    }

    const std::string opt_or_filename = argv[3];

    if (opt_or_filename == "-h" or opt_or_filename == "--help") {
        cli::help_command_fine_tune_upload_file();
        return;
    }

    const std::string id = upload_file(opt_or_filename);
    fmt::print("Success!\nUploaded file: {}\nWith ID: {}\n", opt_or_filename, id);
}

// Create fine tuning job -----------------------------------------------------------------------------------

void create_ft_job(int argc, char **argv)
{
    ParamsFineTune params = cli::get_opts_create_fine_tuning_job(argc, argv);
    print_sep();

    if (params.training_file.has_value()) {
        fmt::print("Training using file with ID: {}\n", params.training_file.value());
    } else {
        throw std::runtime_error("No training file ID provided");
    }

    if (params.model.has_value()) {
        fmt::print("Training model: {}\n", params.model.value());
    } else {
        throw std::runtime_error("No model provided");
    }

    print_sep();

    const std::string id = create_fine_tuning_job(params.model.value(), params.training_file.value());
    fmt::print("Deployed fine tuning job with ID: {}\n", id);
}

// Delete fine tuned model-----------------------------------------------------------------------------------

void delete_fine_tuned_model(int argc, char **argv)
{
    if (argc < 4) {
        cli::help_command_fine_tune_delete_model();
        return;
    }

    const std::string opt_or_model_id = argv[3];

    if (opt_or_model_id == "-h" or opt_or_model_id == "--help") {
        cli::help_command_fine_tune_delete_model();
        return;
    }

    if (delete_model(opt_or_model_id)) {
        fmt::print("Success!\nDeleted model with ID: {}\n", opt_or_model_id);
    } else {
        fmt::print("Warning!\nDid not delete model with ID: {}\n", opt_or_model_id);
    }
}

// Print fine tuning jobs -----------------------------------------------------------------------------------

void unpack_results(const json &results, std::vector<models::FineTuningJob> &jobs)
{
    for (const auto &entry: results["data"]) {
        validation::is_fine_tuning_job(entry);
        models::FineTuningJob job;

        job.id = entry["id"];
        job.created_at = entry["created_at"];

        if (not entry["finished_at"].is_null()) {
            job.finished_at = datetime_from_unix_timestamp(entry["finished_at"]);
        }

        if (not entry["estimated_finish"].is_null()) {
            job.estimated_finish = datetime_from_unix_timestamp(entry["estimated_finish"]);
        }

        jobs.push_back(job);
    }
}

void print_results(const std::vector<models::FineTuningJob> &jobs)
{
    print_sep();
    fmt::print("{:<40}{:<30}{:<30}{}\n", "Job ID", "Created at", "Estimated finish", "Finished at");
    print_sep();

    for (const auto &it: jobs) {
        const std::string dt_created_at = datetime_from_unix_timestamp(it.created_at);
        fmt::print("{:<40}{:<30}{:<30}{}\n", it.id, dt_created_at, it.estimated_finish, it.finished_at);
    }

    print_sep();
}

void list_fine_tuning_jobs(int argc, char **argv)
{
    ParamsGetFineTuningJobs params = cli::get_opts_get_fine_tuning_jobs(argc, argv);
    std::string limit = params.limit.value_or("20");

    OpenAIUser api;
    const std::string response = api.get_fine_tuning_jobs(limit);
    const json results = parse_response(response);

    if (params.print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    validation::is_list(results);

    if (not params.limit.has_value()) {
        print_sep();
        fmt::print("> No limit passed with --limit flag. Will use OpenAI's default retrieval limit of 20 listings\n");
    }

    std::vector<models::FineTuningJob> jobs;
    unpack_results(results, jobs);

    std::sort(jobs.begin(), jobs.end(), [](const models::FineTuningJob &left, const models::FineTuningJob &right) {
        return left.created_at < right.created_at;
    });

    print_results(jobs);
}

} // namespace

void command_fine_tune(int argc, char **argv)
{
    if (argc < 3) {
        cli::help_command_fine_tune();
        exit(EXIT_FAILURE);
    }

    std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        cli::help_command_fine_tune();
        exit(EXIT_SUCCESS);
    }

    if (subcommand == "upload-file") {
        upload_fine_tuning_file(argc, argv);
    } else if (subcommand == "create-job") {
        create_ft_job(argc, argv);
    } else if (subcommand == "delete-model") {
        delete_fine_tuned_model(argc, argv);
    } else if (subcommand == "list-jobs") {
        list_fine_tuning_jobs(argc, argv);
    } else {
        cli::help_command_fine_tune();
        exit(EXIT_FAILURE);
    }
}
