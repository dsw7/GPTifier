#pragma once

#include "typedefs.h"

#include <string>

namespace response
{
void print_response(const ::str_response &response);
void dump_response(const ::str_response &response, const std::string &filename);
} // namespace response
