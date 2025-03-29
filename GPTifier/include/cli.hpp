#pragma once

#include "params.hpp"

namespace cli {

void exit_on_failure();

ParamsRun get_opts_run(int argc, char **argv);
ParamsEmbedding get_opts_embed(int argc, char **argv);
ParamsFineTune get_opts_create_fine_tuning_job(int argc, char **argv);
ParamsGetFineTuningJobs get_opts_get_fine_tuning_jobs(int argc, char **argv);
ParamsGetChatCompletions get_opts_get_chat_completions(int argc, char **argv);

bool get_opts_files_list(int argc, char **argv);

} // namespace cli
