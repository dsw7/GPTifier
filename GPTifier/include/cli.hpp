#pragma once

#include "params.hpp"

namespace cli {

void exit_on_failure();

ParamsGetFineTuningJobs get_opts_get_fine_tuning_jobs(int argc, char **argv);

bool get_opts_files_list(int argc, char **argv);

} // namespace cli
