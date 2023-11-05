# GPTifier
A beautiful C++ libcurl / ChatGPT interface

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/dsw7/GPTifier/master/LICENSE)
## Table of Contents
- [Setup](#setup)
  - [Get `json.hpp`](#get-jsonhpp)
  - [Get `toml.hpp`](#get-tomlhpp)
  - [Compile binary](#compile-binary)
  - [Set up TOML](#set-up-toml)
  - [Clean up](#clean-up)

## Setup
### Get `json.hpp`
This project uses the [nlohmann/json](https://github.com/nlohmann/json) library. The compiler must be able to
locate the `nlohmann/json.hpp` header file. If the `json.hpp` file does not exist anywhere, `cmake` will print
out:
```
-- Checking if nlohmann/json.hpp exists anywhere
-- Checking directory: /usr/include/c++/10
-- Checking directory: /usr/include/x86_64-linux-gnu/c++/10
-- Checking directory: /usr/include/c++/10/backward
-- Checking directory: /usr/lib/gcc/x86_64-linux-gnu/10/include
-- Checking directory: /usr/local/include
-- Checking directory: /usr/include/x86_64-linux-gnu
-- Checking directory: /usr/include
CMake Error at CMakeLists.txt:<line-number> (message):
  Could not find nlohmann/json.hpp in any include directory
```
To install `json.hpp` into say `/usr/include`, first get the file:
```
curl https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp -o /tmp/json.hpp
```
Then create the `nlohmann` directory:
```
sudo mkdir /usr/include/nlohmann
```
Then copy:
```
sudo cp /tmp/json.hpp /usr/include/nlohmann/
```
### Get `toml.hpp`
This project uses the [TOML++](https://marzer.github.io/tomlplusplus/) configuration parser. The compiler must
be able to locate the `toml++/toml.hpp` header file. If the `toml.hpp` file does not exist anywhere, `cmake`
will print out:
```
-- Checking if toml++/toml.hpp exists anywhere
-- Checking directory: /usr/include/c++/10
-- Checking directory: /usr/include/x86_64-linux-gnu/c++/10
-- Checking directory: /usr/include/c++/10/backward
-- Checking directory: /usr/lib/gcc/x86_64-linux-gnu/10/include
-- Checking directory: /usr/local/include
-- Checking directory: /usr/include/x86_64-linux-gnu
-- Checking directory: /usr/include
CMake Error at CMakeLists.txt:<line-number> (message):
  Could not find toml++/toml.hpp in any include directory
```
Which is identical [`json.hpp`](#get-jsonhpp) case. As before, to install `toml.hpp` into `/usr/include`,
first get the file:
```
curl https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp -o toml.hpp
```
Then create the `toml++` directory:
```
sudo mkdir /usr/include/toml++
```
Then copy:
```
sudo cp /tmp/toml.hpp /usr/include/toml++/
```
### Compile binary
To set the product up, simply run the `make` target:
```
make compile
```
The binary will be installed into whatever install directory is resolved by CMake's
[install()](https://cmake.org/cmake/help/latest/command/install.html#command:install).
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
