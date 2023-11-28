#pragma once

#include "data.hpp"

#include <string>

namespace prompt
{

void read_prompt_interactively(std::string &prompt);
void read_prompt_from_file(std::string &prompt, const std::string &filename);
void get_prompt(Params &params);

} // namespace prompt
