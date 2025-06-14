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

option(USE_SYSTEM_NLOHMANN_JSON "Use system-provided nlohmann/json.hpp if available" OFF)

message(STATUS "Checking for nlohmann/json.hpp")

if(USE_SYSTEM_NLOHMANN_JSON)
  # i.e. specified by user as:
  # `cmake -DUSE_SYSTEM_NLOHMANN_JSON=ON -DNLOHMANN_JSON_HPP=/path/to/json.hpp <args>`
  set(NLOHMANN_JSON_HPP "" CACHE FILEPATH "Path to nlohmann/json.hpp")
  if(EXISTS ${NLOHMANN_JSON_HPP})
    message(STATUS "Copying ${NLOHMANN_JSON_HPP} -> ${CMAKE_SOURCE_DIR}/external")
    file(COPY ${NLOHMANN_JSON_HPP} DESTINATION "${CMAKE_SOURCE_DIR}/external")
  else()
    message(FATAL_ERROR "The file '${NLOHMANN_JSON_HPP}' does not exist!")
  endif()
else()
  set(NLOHMANN_JSON_HPP_DL "${CMAKE_SOURCE_DIR}/external/json.hpp")
  if(EXISTS ${NLOHMANN_JSON_HPP_DL})
    message(STATUS "${NLOHMANN_JSON_HPP_DL} already exists. Skipping download")
  else()
    message(STATUS "Downloading nlohmann/json.hpp")
    file(
      DOWNLOAD
      "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
      ${NLOHMANN_JSON_HPP_DL}
      SHOW_PROGRESS
    )
  endif()
endif()

# -----------------------------------------------------------------------------------------------------------
# toml++

option(USE_SYSTEM_TOMLPLUSPLUS "Use system-provided toml++ if available" OFF)

message(STATUS "Checking for toml++ header file")

if(USE_SYSTEM_TOMLPLUSPLUS)
  # i.e. specified by user as:
  # `cmake -DUSE_SYSTEM_TOMLPLUSPLUS=ON -DTOMLPLUSPLUS_HPP=/path/to/toml.hpp <args>`
  set(TOMLPLUSPLUS_HPP "" CACHE FILEPATH "Path to toml.hpp")
  if(EXISTS ${TOMLPLUSPLUS_HPP})
    message(STATUS "Copying ${TOMLPLUSPLUS_HPP} -> ${CMAKE_SOURCE_DIR}/external")
    file(COPY ${TOMLPLUSPLUS_HPP} DESTINATION "${CMAKE_SOURCE_DIR}/external")
  else()
    message(FATAL_ERROR "The file '${TOMLPLUSPLUS_HPP}' does not exist!")
  endif()
else()
  set(TOMLPLUSPLUS_HPP_DL "${CMAKE_SOURCE_DIR}/external/toml.hpp")
  if(EXISTS ${TOMLPLUSPLUS_HPP_DL})
    message(STATUS "${TOMLPLUSPLUS_HPP_DL} already exists. Skipping download")
  else()
    message(STATUS "Downloading toml.hpp")
    file(
      DOWNLOAD
      "https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp"
      ${TOMLPLUSPLUS_HPP_DL}
      SHOW_PROGRESS
    )
  endif()
endif()

# -----------------------------------------------------------------------------------------------------------
# {fmt}

message(STATUS "Locating {fmt} library")
find_package(fmt)

if(NOT fmt_FOUND)
  message(FATAL_ERROR "Is the {fmt} package installed?")
endif()

# -----------------------------------------------------------------------------------------------------------

include_directories("external")
add_executable(
  "gpt"
  "configs.cpp"
  "datadir.cpp"
  "interface/command_chats.cpp"
  "interface/command_costs.cpp"
  "interface/command_edit.cpp"
  "interface/command_embed.cpp"
  "interface/command_files.cpp"
  "interface/command_fine_tune.cpp"
  "interface/command_models.cpp"
  "interface/command_run.cpp"
  "interface/command_short.cpp"
  "interface/command_test.cpp"
  "interface/help_messages.cpp"
  "main.cpp"
  "networking/api_openai_admin.cpp"
  "networking/api_openai_user.cpp"
  "networking/curl_base.cpp"
  "serialization/chat_completions.cpp"
  "serialization/costs.cpp"
  "serialization/embeddings.cpp"
  "serialization/files.cpp"
  "serialization/fine_tuning.cpp"
  "serialization/models.cpp"
  "serialization/response_to_json.cpp"
  "utils.cpp"
)
target_link_libraries("gpt" "curl" "pthread" "fmt::fmt")

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
install(TARGETS "gpt" DESTINATION "bin")
