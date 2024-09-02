#pragma once

#include <string>

namespace Reporting
{

void print_error(const std::string &message);
void print_request(const std::string &request);
void print_response(const std::string &response);

} // namespace Reporting
