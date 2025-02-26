#include "command_fine_tune.hpp"

#include "api_openai_user.hpp"
#include "cli.hpp"
#include "help_messages.hpp"
#include "models.hpp"
#include "params.hpp"
#include "parsers.hpp"
#include "utils.hpp"
#include "validation.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

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

    OpenAIUser api;
    const std::string purpose = "fine-tune";
    const std::string response = api.upload_file(opt_or_filename, purpose);
    const json results = parse_response(response);

    if (not validation::is_file(results)) {
        throw std::runtime_error("Response from OpenAI is not a file");
    }

    const std::string filename = results["filename"];
    const std::string id = results["id"];
    fmt::print("Success!\nUploaded file: {}\nWith ID: {}\n", filename, id);
}

void create_fine_tuning_job(int argc, char **argv)
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

    const json data = { { "model", params.model.value() }, { "training_file", params.training_file.value() } };

    OpenAIUser api;
    const std::string response = api.create_fine_tuning_job(data.dump());
    const json results = parse_response(response);

    if (not validation::is_fine_tuning_job(results)) {
        throw std::runtime_error("Response from OpenAI is not a fine-tuning job object");
    }

    fmt::print("Deployed fine tuning job with ID: {}\n", results["id"]);
}

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

    OpenAIUser api;
    const std::string response = api.delete_model(opt_or_model_id);
    const json results = parse_response(response);

    if (not validation::is_model(results)) {
        throw std::runtime_error("Response from OpenAI is not a model");
    }

    if (results["deleted"]) {
        fmt::print("Success!\nDeleted model with ID: {}\n", results["id"]);
    } else {
        fmt::print("Warning!\nDid not delete model with ID: {}\n", results["id"]);
    }
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

    if (not validation::is_fine_tuning_jobs_list(results)) {
        throw std::runtime_error("Response from OpenAI is not a list of fine-tuning jobs");
    }

    if (not params.limit.has_value()) {
        print_sep();
        fmt::print("> No limit passed with --limit flag. Will use OpenAI's default retrieval limit of 20 listings\n");
    }

    print_sep();
    fmt::print("{:<40}{:<30}{:<30}{}\n", "Job ID", "Created at", "Estimated finish", "Finished at");

    print_sep();
    std::vector<models::Job> jobs;

    for (const auto &entry: results["data"]) {
        models::Job job;

        job.id = entry["id"];
        job.created_at = entry["created_at"];

        if (not entry["finished_at"].is_null()) {
            job.finished_at = entry["finished_at"];
        }

        if (not entry["estimated_finish"].is_null()) {
            job.estimated_finish = entry["estimated_finish"];
        }

        jobs.push_back(job);
    }

    models::sort(jobs);

    for (auto it = jobs.begin(); it != jobs.end(); ++it) {
        it->print();
    }

    print_sep();
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
        create_fine_tuning_job(argc, argv);
    } else if (subcommand == "delete-model") {
        delete_fine_tuned_model(argc, argv);
    } else if (subcommand == "list-jobs") {
        list_fine_tuning_jobs(argc, argv);
    } else {
        cli::help_command_fine_tune();
        exit(EXIT_FAILURE);
    }
}
