#include <iostream>
#include <string>

void help(const std::string &exec)
{
    std::cerr << "--- GPTifier ---\n";
    std::cerr << "Usage: " + exec + " '<prompt>'" << std::endl;
}

int main(int argc, char **argv)
{
    std::string exec = std::string(argv[0]);

    if (argc < 2)
    {
        ::help(exec);
        return EXIT_FAILURE;
    }

    std::string prompt = std::string(argv[1]);

    if (prompt.compare("-h") == 0)
    {
        ::help(exec);
        return EXIT_SUCCESS;
    }

    if (prompt.compare("--help") == 0)
    {
        ::help(exec);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
