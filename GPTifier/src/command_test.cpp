#include "command_test.hpp"

#include <fmt/core.h>
#include <stdexcept>

void command_test(int argc, char **argv)
{
    if (argc < 3) {
        throw std::runtime_error("Usage: gpt test (ccc | mod)");
    }

    const std::string command = argv[2];
    fmt::print("{}\n", command);
}
