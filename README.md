# GPTifier
A beautiful C++ libcurl / ChatGPT interface

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/dsw7/GPTifier/master/LICENSE)
[![GitHub Releases](https://img.shields.io/github/release/dsw7/GPTifier.svg)](https://github.com/dsw7/GPTifier/releases)

## Table of Contents
- [Motivation](#motivation)
- [Setup](#setup)
  - [Get specific release](#get-specific-release)
  - [Compile binary](#compile-binary)
  - [Get `json.hpp`](#get-jsonhpp)
  - [Get `toml.hpp`](#get-tomlhpp)
  - [Set up TOML](#set-up-toml)
  - [Clean up](#clean-up)

## Motivation
There are numerous ChatGPT command line programs currently available. Many of them are written in Python. I
wanted something a bit quicker and a bit easier to install, so I wrote this program in C++.

## Setup
### Get specific release
Interested in a specific release? To download `1.0.0`, for example:
```
wget https://github.com/dsw7/GPTifier/archive/refs/tags/v1.0.0.tar.gz
```
Which will yield:
```
v1.0.0.tar.gz
```
Then run:
```
tar -xvf v1.0.0.tar.gz
```
Which will generate:
```
GPTifier-1.0.0
```
Change directories into `GPTifier-1.0.0` and proceed with the next steps.
### Compile binary
To set the product up, simply run the `make` target:
```
make compile
```
The binary will be installed into whatever install directory is resolved by CMake's
[install()](https://cmake.org/cmake/help/latest/command/install.html#command:install).
### Get `json.hpp`
This project uses the [nlohmann/json](https://github.com/nlohmann/json) library. The compiler must be able to
locate the `json.hpp` header file. If the `json.hpp` file does not exist anywhere, `cmake` will print out:
```
-- Checking if json.hpp exists anywhere
-- Checking directory: /usr/include/c++/10
-- Checking directory: /usr/include/x86_64-linux-gnu/c++/10
-- Checking directory: /usr/include/c++/10/backward
-- Checking directory: /usr/lib/gcc/x86_64-linux-gnu/10/include
-- Checking directory: /usr/local/include
-- Checking directory: /usr/include/x86_64-linux-gnu
-- Checking directory: /usr/include
CMake Error at CMakeLists.txt:<line-number> (message):
  Could not find json.hpp in any include directory
```
To install `json.hpp` into say `/usr/include`, simply run the convenience script:
```
./get_deps.sh /usr/include/
```
Running the script may require elevated privileges.
### Get `toml.hpp`
This project uses the [TOML++](https://marzer.github.io/tomlplusplus/) configuration parser. The compiler must
be able to locate the `toml.hpp` header file. If the `toml.hpp` file does not exist anywhere, `cmake` will
print out:
```
-- Checking if toml.hpp exists anywhere
-- Checking directory: /usr/include/c++/10
-- Checking directory: /usr/include/x86_64-linux-gnu/c++/10
-- Checking directory: /usr/include/c++/10/backward
-- Checking directory: /usr/lib/gcc/x86_64-linux-gnu/10/include
-- Checking directory: /usr/local/include
-- Checking directory: /usr/include/x86_64-linux-gnu
-- Checking directory: /usr/include
CMake Error at CMakeLists.txt:<line-number> (message):
  Could not find toml.hpp in any include directory
```
Which is identical [`json.hpp`](#get-jsonhpp) case. As before, simply run the convenience script:
```
./get_deps.sh /usr/include/
```
Running the script may require elevated privileges.
### Set up TOML
This project uses [TOML](https://toml.io/en/) to store configurations. Copy the [.gptifier](./.gptifier) TOML
file from this repository to your home directory:
```
cp .gptifier ~/
```
Set the permissions to 600:
```
chmod 600 ~/.gptifier
```
And edit the file:
```toml
[authentication]
# Specify API key
# See https://platform.openai.com/docs/api-reference/authentication
api-key = "<your-api-key>"

[models]
# Specify model
# See https://platform.openai.com/docs/models/overview for a list of models
model = "<choose-a-valid-model>"
```
Next, drop into the program:
```
gpt
```
The program should begin if the `.gptifier` file is properly set up.
### Clean up
The compilation process will generate many build artifacts. Clean up the build artifacts by running:
```
make clean
```
