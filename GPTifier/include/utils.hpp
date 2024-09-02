#pragma once

#include <ctime>
#include <string>

void print_separator();
std::string get_proj_home_dir();
std::string datetime_from_unix_timestamp(const std::time_t &timestamp);
void read_text_from_file(const std::string &filename, std::string &text);
