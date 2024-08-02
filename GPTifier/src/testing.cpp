#include "testing.hpp"

#include <fmt/core.h>

#include <cstdlib>
#include <iostream>

bool is_pytest_running()
{
    const char *pytest_current_test = std::getenv("PYTEST_CURRENT_TEST");

    if (pytest_current_test)
    {
        std::cout << fmt::format("Detected pytest run ({})\n", pytest_current_test);
        return true;
    }

    return false;
}

bool is_test_running()
{
    static bool pytest_running = ::is_pytest_running();
    return pytest_running;
}
