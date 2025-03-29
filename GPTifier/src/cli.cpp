#include "cli.hpp"

#include <fmt/core.h>

namespace cli {

void exit_on_failure()
{
    fmt::print(stderr, "Try running with -h or --help for more information\n");
    exit(EXIT_FAILURE);
}

} // namespace cli
