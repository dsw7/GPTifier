cmake_minimum_required(VERSION 3.18)
project(GPTifier VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")
add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")

add_compile_options(-Wall -Wextra -pedantic -Werror)

set(NLOHMANN_JSON "json.hpp")
set(NLOHMANN_JSON_NOT_FOUND BOOL TRUE)

message(STATUS "Checking if ${NLOHMANN_JSON} exists anywhere")

foreach(dir ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  message(STATUS "Checking directory: ${dir}")
  if(EXISTS "${dir}/${NLOHMANN_JSON}")
    message(STATUS "File ${NLOHMANN_JSON} found in ${dir}")
    set(NLOHMANN_JSON_NOT_FOUND FALSE)
    break()
  endif()
endforeach()

if(NLOHMANN_JSON_NOT_FOUND)
  message(FATAL_ERROR "Could not find ${NLOHMANN_JSON} in any include directory")
endif()

set(TOMLPLUSPLUS "toml.hpp")
set(TOMLPLUSPLUS_NOT_FOUND BOOL TRUE)

message(STATUS "Checking if ${TOMLPLUSPLUS} exists anywhere")

foreach(dir ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  message(STATUS "Checking directory: ${dir}")
  if(EXISTS "${dir}/${TOMLPLUSPLUS}")
    message(STATUS "File ${TOMLPLUSPLUS} found in ${dir}")
    set(TOMLPLUSPLUS_NOT_FOUND FALSE)
    break()
  endif()
endforeach()

if(TOMLPLUSPLUS_NOT_FOUND)
  message(FATAL_ERROR "Could not find ${TOMLPLUSPLUS} in any include directory")
endif()

include_directories(
  "include"
)
add_executable(
  "gpt"
  "src/help_messages.cpp"
  "src/main.cpp"
  "src/params.cpp"
  "src/prompts.cpp"
  "src/query.cpp"
  "src/requests.cpp"
  "src/responses.cpp"
  "src/utils.cpp"
)
target_link_libraries(
  "gpt"
  "curl"
  "pthread"
)

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
install(TARGETS "gpt" DESTINATION "bin")
