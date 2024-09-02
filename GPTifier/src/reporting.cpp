#include <fmt/core.h>
#include <iostream>

namespace Reporting
{

void print_error(const std::string &message)
{
    std::cout << fmt::format("\033[1mError:\033[31m {}\033[0m\n", message);
}

void print_request(const std::string &request)
{
    std::cout << fmt::format("\033[1mRequest:\033[0m {}\n", request);
}

void print_response(const std::string &response)
{
    std::cout << fmt::format("\033[1mResponse:\033[0m {}\n", response);
}

} // namespace Reporting
