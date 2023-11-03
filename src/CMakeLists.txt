project(GPTifier)
cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_executable(
  "gpt"
  "main.cpp"
  "query.cpp"
)
target_link_libraries(
  "gpt"
  "curl"
)
