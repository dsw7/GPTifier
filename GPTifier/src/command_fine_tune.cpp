#include "command_fine_tune.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "help_messages.hpp"
#include "json.hpp"
#include "parsers.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <map>
#include <string>

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

    const std::string purpose = "fine-tune";
    const std::string response = api::upload_file(opt_or_filename, purpose);

    const nlohmann::json results = parse_response(response);

    const std::string filename = results["filename"];
    const std::string id = results["id"];

    fmt::print("Success!\nUploaded file: {}\nWith ID: {}\n", filename, id);
}

void create_fine_tuning_job(int argc, char **argv)
{
    cli::ParamsFineTune params = cli::get_opts_create_fine_tuning_job(argc, argv);

    reporting::print_sep();

    if (params.training_file.has_value()) {
        fmt::print("Training using file with ID: {}\n", params.training_file.value());
    } else {
        reporting::print_error("No training file ID provided");
        return;
    }

    if (params.model.has_value()) {
        fmt::print("Training model: {}\n", params.model.value());
    } else {
        reporting::print_error("No model provided");
        return;
    }

    reporting::print_sep();

    const std::string response = api::create_fine_tuning_job(params.training_file.value(), params.model.value());
    const nlohmann::json results = parse_response(response);

    const std::string id = results["id"];
    fmt::print("Deployed fine tuning job with ID: {}\n", id);
}

void delete_fine_tuned_model(int argc, char **argv)
{
    if (argc < 4) {
        cli::help_command_fine_tune_delete_model();
        return;
    }

    const std::string opt_or_model = argv[3];

    if (opt_or_model == "-h" or opt_or_model == "--help") {
        cli::help_command_fine_tune_delete_model();
        return;
    }

    const std::string response = api::delete_model(opt_or_model);
    const nlohmann::json results = parse_response(response);

    const std::string id = results["id"];

    if (results["deleted"]) {
        fmt::print("Success!\nDeleted model with ID: {}\n", id);
    } else {
        fmt::print("Warning!\nDid not delete model with ID: {}\n", id);
    }
}

struct Job {
    int estimated_finish;
    int finished_at;
    std::string id;
};

void print_jobs(int created_at, const Job &job)
{
    const std::string datetime = datetime_from_unix_timestamp(created_at);
    fmt::print("{:<30}{:<30}{:<30}{}\n", job.id, datetime, job.finished_at, job.estimated_finish);
}

void list_fine_tuning_jobs(int argc, char **argv)
{
    cli::ParamsGetFineTuningJobs params = cli::get_opts_get_fine_tuning_jobs(argc, argv);
    std::string limit = params.limit.value_or("20");

    const std::string response = api::get_fine_tuning_jobs(limit);
    const nlohmann::json results = parse_response(response);

    if (params.print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    reporting::print_sep();
    fmt::print("{:<30}{:<30}{:<30}{}\n", "Job ID", "Created at", "Finished at", "Estimated finish");

    reporting::print_sep();
    std::map<int, Job> jobs = {};

    for (const auto &entry: results["data"]) {
        Job job;
        job.id = entry["id"];
        job.finished_at = entry.value("finished_at", -1);

        if (entry["estimated_finish"].is_null()) {
            job.estimated_finish = -1;
        } else {
            job.estimated_finish = entry["estimated_finish"];
        }

        jobs[entry["created_at"]] = job;
    }

    for (auto it = jobs.begin(); it != jobs.end(); ++it) {
        print_jobs(it->first, it->second);
    }

    reporting::print_sep();
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
