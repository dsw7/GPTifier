project(GPTifier)
set(CMAKE_CXX_STANDARD 17)

add_executable(
  "gpt"
  "main.cpp"
  "query.cpp"
)
target_link_libraries(
  "gpt"
  "curl"
)
