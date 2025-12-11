cmake_minimum_required(VERSION 3.18)
project(GPTifier VERSION 1.7.0)

# Keep all options up top
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(ENABLE_TESTING "Set the TESTING_ENABLED macro" OFF)
option(USE_SYSTEM_NLOHMANN_JSON "Use system-provided nlohmann/json.hpp if available" OFF)
option(USE_SYSTEM_TOMLPLUSPLUS "Use system-provided toml++ if available" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local/bin")

string(TIMESTAMP BUILD_DATE)
string(TIMESTAMP CURRENT_YEAR "%Y")

add_compile_options(-Wall -Wextra -pedantic -Werror)
add_compile_definitions(
  BUILD_DATE="${BUILD_DATE}"
  CURRENT_YEAR="${CURRENT_YEAR}"
  PROJECT_VERSION="${PROJECT_VERSION}"
)

if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

if(ENABLE_TESTING)
  add_compile_definitions(TESTING_ENABLED)
endif()

include_directories(
  external
  src
  src/commands
  src/networking
  src/serialization
)

set(SRC_FILES
  src/commands/command_costs.cpp
  src/commands/command_embed.cpp
  src/commands/command_files.cpp
  src/commands/command_fine_tune.cpp
  src/commands/command_img.cpp
  src/commands/command_models.cpp
  src/commands/command_run.cpp
  src/commands/command_short.cpp
  src/commands/command_test.cpp
  src/configs.cpp
  src/datadir.cpp
  src/main.cpp
  src/networking/api_openai_admin.cpp
  src/networking/api_openai_user.cpp
  src/networking/curl_base.cpp
  src/serialization/costs.cpp
  src/serialization/embeddings.cpp
  src/serialization/files.cpp
  src/serialization/fine_tuning.cpp
  src/serialization/images.cpp
  src/serialization/models.cpp
  src/serialization/response_to_json.cpp
  src/serialization/responses.cpp
  src/serialization/ser_utils.cpp
  src/serialization/testing.cpp
  src/utils.cpp
)

# -----------------------------------------------------------------------------------------------------------
# External dependencies

message(STATUS "Locating {fmt} library")
find_package(fmt)

if(NOT fmt_FOUND)
  message(FATAL_ERROR "Is the {fmt} package installed?")
endif()

# nlohmann/json.hpp
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

# toml++
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
add_executable(gpt ${SRC_FILES})
target_link_libraries(gpt curl pthread fmt::fmt)
install(TARGETS gpt DESTINATION ${CMAKE_INSTALL_PREFIX})
