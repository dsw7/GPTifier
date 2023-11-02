#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " + std::string(argv[0]) + " '<prompt>'" << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
