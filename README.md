# GPTifier
A beautiful C++ libcurl / ChatGPT interface
## Table of Contents
- [Setup](#setup)
  - [Get `json.hpp`](#get-json-hpp)
  - [Compile binary](#compile-binary)

## Setup
### Get `json.hpp`
This project uses the [nlohmann/json](https://github.com/nlohmann/json) library. The compiler must be able to locate the `nlohmann/json.hpp` header file. To set up the header file, first run:
```
curl https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp -o /tmp/json.hpp
```
### Compile binary
To set the product up, simply run the `make` target:
```
make compile
```
