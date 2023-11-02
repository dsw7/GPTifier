project(GPTifier)
set(CMAKE_CXX_STANDARD 17)

add_executable(
  "gpt"
  "main.cpp"
)
target_link_libraries(
  "gpt"
  "curl"
)
