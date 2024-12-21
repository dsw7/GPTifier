#include "command_fine_tune.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "help_messages.hpp"
#include "json.hpp"
#include "reporting.hpp"

#include <fmt/core.h>
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
    const std::string response = query_upload_file_api(opt_or_filename, purpose);
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error")) {
        const std::string error = results["error"]["message"];
        reporting::print_error(error);
        return;
    }

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

    const std::string response = query_create_fine_tuning_job_api(params.training_file.value(), params.model.value());
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error")) {
        if (not results["error"].empty()) {
            const std::string error = results["error"]["message"];
            reporting::print_error(error);
            return;
        }
    }

    const std::string id = results["id"];
    fmt::print("Deployed fine tuning job with ID: {}\n", id);
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
    } else {
        cli::help_command_fine_tune();
        exit(EXIT_FAILURE);
    }
}
