#include "command_test.hpp"

#include <stdexcept>

namespace {

void test_create_chat_completion_api()
{
}

void test_models_api()
{
}

} // namespace

void command_test(int argc, char **argv)
{
    if (argc < 3) {
        throw std::runtime_error("Usage: gpt test (ccc | mod)");
    }

    const std::string target = argv[2];

    if (target == "ccc") {
        test_create_chat_completion_api();
    } else if (target == "mod") {
        test_models_api();
    } else {
        throw std::runtime_error("Unknown test target: " + target);
    }
}
