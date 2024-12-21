#include <fmt/core.h>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

namespace {

unsigned short get_terminal_columns()
{
    static struct winsize window_size;
    window_size.ws_col = 0;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == 0) {
        return window_size.ws_col;
    }

    return 20;
}

} // namespace

namespace reporting {

void print_sep()
{
    static unsigned short columns = get_terminal_columns();
    std::cout << std::string(columns, '-') + "\n";
}

void print_error(const std::string &message)
{
    std::cout << fmt::format("\033[1mError:\033[31m {}\033[0m\n", message);
}

void print_response(const std::string &response)
{
    std::cout << fmt::format("\033[1mResponse:\033[0m {}\n", response);
}

void print_results(const std::string &results)
{
    std::cout << fmt::format("\033[1mResults:\033[32m {}\033[0m\n", results);
}

} // namespace reporting
