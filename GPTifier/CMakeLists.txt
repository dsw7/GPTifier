cmake_minimum_required(VERSION 3.18)
project(GPTifier VERSION 1.4.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

string(TIMESTAMP BUILD_DATE)
message(STATUS "Will add the following timestamp to binary: ${BUILD_DATE}")

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")
add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
add_compile_definitions(BUILD_DATE="${BUILD_DATE}")

add_compile_options(-Wall -Wextra -pedantic -Werror)

# -----------------------------------------------------------------------------------------------------------
# Testing

option(ENABLE_TESTING "Set the TESTING_ENABLED macro" OFF)

if(ENABLE_TESTING)
  add_compile_definitions(TESTING_ENABLED)
endif()

# -----------------------------------------------------------------------------------------------------------
# nlohmann/json.hpp

message(STATUS "Checking for nlohmann/json.hpp")

set(NLOHMANN_JSON_HPP "${CMAKE_SOURCE_DIR}/external/json.hpp")

if(NOT EXISTS ${NLOHMANN_JSON_HPP})
  message(STATUS "Downloading nlohmann/json.hpp")
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
    ${NLOHMANN_JSON_HPP}
    SHOW_PROGRESS
  )
else()
  message(STATUS "nlohmann/json.hpp already exists. Skipping download")
endif()

# -----------------------------------------------------------------------------------------------------------
# toml++

message(STATUS "Checking for toml++ header file")

set(TOMLPLUSPLUS_HPP "${CMAKE_SOURCE_DIR}/external/toml.hpp")

if(NOT EXISTS ${TOMLPLUSPLUS_HPP})
  message(STATUS "Downloading toml.hpp")
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp"
    ${TOMLPLUSPLUS_HPP}
    SHOW_PROGRESS
  )
else()
  message(STATUS "toml.hpp already exists. Skipping download")
endif()

# -----------------------------------------------------------------------------------------------------------
# {fmt}

message(STATUS "Locating {fmt} library")
find_package(fmt)

if(NOT fmt_FOUND)
  message(FATAL_ERROR "Is the {fmt} package installed?")
endif()

# -----------------------------------------------------------------------------------------------------------

include_directories(
  "external"
  "include"
)
add_executable(
  "gpt"
  "src/configs.cpp"
  "src/datadir.cpp"
  "src/interface/command_chats.cpp"
  "src/interface/command_costs.cpp"
  "src/interface/command_edit.cpp"
  "src/interface/command_embed.cpp"
  "src/interface/command_files.cpp"
  "src/interface/command_fine_tune.cpp"
  "src/interface/command_models.cpp"
  "src/interface/command_run.cpp"
  "src/interface/command_short.cpp"
  "src/interface/command_test.cpp"
  "src/interface/help_messages.cpp"
  "src/interface/model_selector.cpp"
  "src/main.cpp"
  "src/networking/api_openai_admin.cpp"
  "src/networking/api_openai_user.cpp"
  "src/networking/curl_base.cpp"
  "src/serialization/chat_completions.cpp"
  "src/serialization/costs.cpp"
  "src/serialization/embeddings.cpp"
  "src/serialization/files.cpp"
  "src/serialization/fine_tuning.cpp"
  "src/serialization/models.cpp"
  "src/serialization/response_to_json.cpp"
  "src/utils.cpp"
)
target_link_libraries(
  "gpt"
  "curl"
  "pthread"
  "fmt::fmt"
)

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
install(TARGETS "gpt" DESTINATION "bin")
