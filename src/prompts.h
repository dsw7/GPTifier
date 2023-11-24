#pragma once

#include "data.h"

#include <string>

namespace prompt
{

void read_prompt_interactively(std::string &prompt);
void get_prompt(Params &params);

} // namespace prompt
