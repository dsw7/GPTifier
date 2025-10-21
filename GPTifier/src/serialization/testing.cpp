#include "testing.hpp"

#include <iostream>

namespace serialization {

// NOLINTBEGIN
void test_catch_memory_leak()
{
    int *val = new int(5);
    std::cout << *val << '\n';
}
// NOLINTEND

} // namespace serialization
