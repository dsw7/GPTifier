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
    std::cout << std::string(columns, '-') + "\n";
}

void print_results(const std::string &reply)
{
    nlohmann::json results = nlohmann::json::parse(reply);

    print_separator();

    if (results.contains("error"))
    {
        std::string error = results["error"]["message"];
        results["error"]["message"] = "<See Results section>";

        std::cout << "\033[1mResponse:\033[0m " + results.dump(2) + "\n";
        print_separator();

        std::cout << "\033[1mResults:\033[31m " + error + "\033[0m\n";
        print_separator();

        std::cout << std::endl;
    }
    else
    {
        std::string content = results["choices"][0]["message"]["content"];
        results["choices"][0]["message"]["content"] = "<See Results section>";

        std::cout << "\033[1mResponse:\033[0m " + results.dump(2) + "\n";
        print_separator();

        std::cout << "\033[1mResults:\033[32m " + content + "\033[0m\n";
        print_separator();

        std::cout << std::endl;
    }
}

} // presentation
