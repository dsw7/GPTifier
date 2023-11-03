#include "presentation.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <sys/ioctl.h>
#include <unistd.h>

unsigned short get_terminal_columns()
{
    static struct ::winsize window_size;
    window_size.ws_col = 0; // handle 'Conditional jump or move depends on uninitialized value(s)'

    if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == 0)
    {
        return window_size.ws_col;
    }

    return 20;
}

namespace presentation
{

void print_separator()
{
    static unsigned short columns = ::get_terminal_columns();
    std::cout << std::string(columns, '-') << std::endl;
}

void print_help(const std::string &exec)
{
    print_separator();
    std::cout << "Usage: " + exec + " '<prompt>'" << std::endl;
}

void print_results(const std::string &reply)
{
    nlohmann::json results = nlohmann::json::parse(reply);

    print_separator();
    std::string content = results["choices"][0]["message"]["content"];
    std::cout << content << std::endl;

    print_separator();
    std::cout << results.dump(2) << std::endl;
}

} // presentation
