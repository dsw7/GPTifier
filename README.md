# GPTifier
A beautiful C++ libcurl / ChatGPT interface
## Table of Contents
- [Setup](#setup)
  - [Get `json.hpp`](#get-jsonhpp)
  - [Compile binary](#compile-binary)
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
### Compile binary
To set the product up, simply run the `make` target:
```
make compile
```
The binary will be installed into whatever install directory is resolved by CMake's
[install()](https://cmake.org/cmake/help/latest/command/install.html#command:install). To drop into the
program, run:
```
gpt
```
### Clean up
The compilation process will generate many build artifacts. Clean up the build artifacts by running:
```
make clean
```
