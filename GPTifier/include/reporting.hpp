#pragma once

#include <string>

namespace Reporting
{

void print_sep();
void print_error(const std::string &message);
void print_request(const std::string &request);
void print_response(const std::string &response);
void print_results(const std::string &results);

} // namespace Reporting
