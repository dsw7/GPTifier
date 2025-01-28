#include "reporting.hpp"

#include "utils.hpp"

#include <fmt/core.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace reporting {

void print_error(const std::string &message)
{
    fmt::print(fg(white), "Error: ");
    fmt::print(fg(red), "{}\n", message);
}

} // namespace reporting
