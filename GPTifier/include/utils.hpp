#pragma once

#include <ctime>
#include <string>

std::string datetime_from_unix_timestamp(const std::time_t &timestamp);
std::string read_text_from_file(const std::string &filename);
