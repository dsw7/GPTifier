#include "cli.h"

#include <cstdlib>
#include <getopt.h>

namespace cli
{

void parse_cli(const int argc, char **argv)
{
    int option;

    while (true)
    {
        static struct option long_options[] =
        {
            {"help",        no_argument,       0, 'h'},
            {"version",     no_argument,       0, 'v'},
            {0,             0,                 0,  0 }
        };

        int option_index = 0;

        option = ::getopt_long(
            argc, argv, "hv", long_options, &option_index
        );

        if (option == -1)
        {
            break;
        }

        switch (option)
        {
            case 'h':
                //::help_message();
                ::exit(EXIT_SUCCESS);
            case 'v':
                //::help_message();
                ::exit(EXIT_SUCCESS);
            default:
                //::help_message();
                ::exit(EXIT_SUCCESS);
        }
    };
}

} // cli
