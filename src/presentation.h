#pragma once

#include <string>

unsigned short get_terminal_columns();

namespace presentation
{
    void print_separator();
    void print_help(const std::string &exec);
}
