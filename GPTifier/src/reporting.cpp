#include <fmt/color.h>
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
    static std::string separator = std::string(columns, '-');
    fmt::print("{}\n", separator);
}

void print_error(const std::string &message)
{
    fmt::print(fg(fmt::terminal_color::bright_white), "Error: ");
    fmt::print(fg(fmt::terminal_color::bright_red), "{}\n", message);
}

void print_response(const std::string &response)
{
    fmt::print(fg(fmt::terminal_color::bright_white), "Response: {}\n", response);
}

void print_results(const std::string &results)
{
    fmt::print(fg(fmt::terminal_color::bright_white), "Results: ");
    fmt::print(fg(fmt::terminal_color::bright_green), "{}\n", results);
}

} // namespace reporting
