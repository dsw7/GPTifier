#include "configs.h"

#include <toml++/toml.hpp>

void read_configs(Configs &configs)
{
    configs.api_key = "foo";
    configs.model = "foo";
}
