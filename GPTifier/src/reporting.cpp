#include <fmt/core.h>
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
    fmt::print("{}\n", std::string(columns, '-'));
}

void print_error(const std::string &message)
{
    fmt::print("\033[1mError:\033[31m {}\033[0m\n", message);
}

void print_response(const std::string &response)
{
    fmt::print("\033[1mResponse:\033[0m {}\n", response);
}

void print_results(const std::string &results)
{
    fmt::print("\033[1mResults:\033[32m {}\033[0m\n", results);
}

} // namespace reporting
