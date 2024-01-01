#pragma once

#include "configs.hpp"
#include "data.hpp"

namespace cli
{
void parse_cli(const int argc, char **argv, Params &params);
}
