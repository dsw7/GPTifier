#include <fmt/core.h>
#include <iostream>

namespace Reporting
{

void print_error(const std::string &message)
{
    std::cout << fmt::format("\033[1mError:\033[31m {}\033[0m\n", message);
}

} // namespace Reporting
