#pragma once

#include "typedefs.hpp"

namespace requests
{
::str_request build_chat_request();
void print_request(const ::str_request &request);
} // namespace requests
