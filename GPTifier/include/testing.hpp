#pragma once

#include <string>

namespace testing {
bool is_test_running();
void log_test(const std::string &message);
} // namespace testing
