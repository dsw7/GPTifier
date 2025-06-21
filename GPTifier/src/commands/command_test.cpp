#include "command_test.hpp"

#include "testing.hpp"

#include <fmt/core.h>
#include <stdexcept>

namespace commands {

void command_test(int argc, char **argv)
{
    if (argc == 2) {
        throw std::runtime_error("Usage: gpt test (<target>)");
    }

    const std::string target = argv[2];

    if (target == "leak") {
        serialization::test_catch_memory_leak();
    } else if (target == "ccc") {
        fmt::print("{}\n", serialization::test_curl_handle_is_reusable());
    } else {
        throw std::runtime_error("Unknown test target: " + target);
    }
}

} // namespace commands
