#pragma once

#include "params.hpp"

namespace cli {

ParamsRun get_opts_run(int argc, char **argv);
ParamsShort get_opts_short(int argc, char **argv);
ParamsModels get_opts_models(int argc, char **argv);
ParamsEmbedding get_opts_embed(int argc, char **argv);
ParamsFineTune get_opts_create_fine_tuning_job(int argc, char **argv);
ParamsGetFineTuningJobs get_opts_get_fine_tuning_jobs(int argc, char **argv);
ParamsCosts get_opts_get_costs(int argc, char **argv);
ParamsGetChatCompletions get_opts_get_chat_completions(int argc, char **argv);

bool get_opts_files_list(int argc, char **argv);

} // namespace cli
