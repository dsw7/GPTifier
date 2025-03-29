#pragma once

namespace cli {
void exit_on_failure();
void help_root_messages();
void help_command_embed();
void help_command_models();
void help_command_run();
void help_command_short();
void help_command_files();
void help_command_files_list();
void help_command_files_delete();
void help_command_fine_tune();
void help_command_fine_tune_upload_file();
void help_command_fine_tune_create_job();
void help_command_fine_tune_delete_model();
void help_command_fine_tune_list_jobs();
void help_command_costs();
void help_command_chats();
void help_command_chats_list();
void help_command_chats_delete();
} // namespace cli
