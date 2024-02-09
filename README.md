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
  - [Set up project files](#set-up-project-files)
  - [Clean up](#clean-up)
- [Usage](#usage)
  - [Basic example](#basic-example)
  - [Exporting a result](#exporting-a-result)
- [Integrations](#integrations)
  - [Coupling with `vim`](#coupling-with-vim)
- [Development](#development)
  - [Testing](#testing)
  - [Formatting](#formatting)
  - [Linting](#linting)

## Motivation
There are numerous ChatGPT command line programs currently available. Many of them are written in Python. I
wanted something a bit quicker and a bit easier to install, so I wrote this program in C++.

## Setup
### Get specific release
Interested in a specific release? To download `1.0.0`, for example:
```console
wget https://github.com/dsw7/GPTifier/archive/refs/tags/v1.0.0.tar.gz
```
Which will yield:
```console
v1.0.0.tar.gz
```
Then run:
```console
tar -xvf v1.0.0.tar.gz
```
Which will generate:
```console
GPTifier-1.0.0
```
Change directories into `GPTifier-1.0.0` and proceed with the next steps.
### Compile binary
To set the product up, simply run the `make` target:
```console
make compile
```
The binary will be installed into whatever install directory is resolved by CMake's
[install()](https://cmake.org/cmake/help/latest/command/install.html#command:install).
### Get `json.hpp`
This project uses the [nlohmann/json](https://github.com/nlohmann/json) library. The compiler must be able to
locate the `json.hpp` header file. If the `json.hpp` file does not exist anywhere, `cmake` will print out:
```console
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
```console
./get_dependencies /usr/include/
```
Running the script may require elevated privileges.
### Get `toml.hpp`
This project uses the [TOML++](https://marzer.github.io/tomlplusplus/) configuration parser. The compiler must
be able to locate the `toml.hpp` header file. If the `toml.hpp` file does not exist anywhere, `cmake` will
print out:
```console
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
```console
./get_dependencies /usr/include/
```
Running the script may require elevated privileges.
### Set up project files
This project makes reference to a "home directory" that must be set up prior to running the program. To set up
the home directory, run:
```console
./setup
```
This script will drop a configuration file into the project's home home directory. Open the file:
```console
~/.gptifier/gptifier.toml
```
And apply the relevant configurations. Next, drop into the program:
```console
gpt
```
The program should begin if the `.gptifier` file is properly set up.
### Clean up
The compilation process will generate many build artifacts. Clean up the build artifacts by running:
```console
make clean
```

## Usage
### Basic example
Simply run `gpt`! This will begin an interactive session. Type in a prompt:
```console
$ gpt
------------------------------------------------------------------------------------------
Input: What is 3 + 5?
```
And hit `Enter`. The program will dispatch a request and return:
```console
...
Results: 3 + 5 equals 8.
------------------------------------------------------------------------------------------
Export:
> Write reply to file? [y/n]:
```
### Exporting a result
In the above example, the user is prompted to export the completion a file. Entering `y` will print:
```console
...
> Writing reply to file /home/<your-username>/results.gpt
------------------------------------------------------------------------------------------
```
Subsequent requests will append to this file. A suggested practice is to "highlight" this file by updating the
`LS_COLORS` environment variable in any `init` file as follows:
```console
LS_COLORS=$LS_COLORS:"*.gpt=4;93"
export LS_COLORS
```
In some cases, prompting interactively may be undesirable, such as when running automated unit tests. To
disable the `y/n` prompt, run `gpt` with the `-u` or `--no-interactive-export` flags.

## Integrations
### Coupling with `vim`
In the [Exporting a result](#exporting-a-result) section, it was stated that results can be voluntarily
exported to `~/results.gpt`. One may be interested in integrating this into a `vim` workflow. This can be
achieved as follows. First, add the following function to `~/.vimrc`:
```vim
function OpenGPTifierResults()
  let l:results_file = expand('~') . '/results.gpt'

  if filereadable(l:results_file)
    execute 'vs' . l:results_file
  else
    echoerr l:results_file . ' does not exist'
  endif
endfunction
```
Then add a command to `~/.vimrc`:
```vim
" Open GPTifier results file
command G :call OpenGPTifierResults()
```
The command `G` will open `~/results.gpt` in a separate vertical split, thus allowing for cherry picking saved
OpenAI completions into a source file, for example.

## Development
### Testing
To run unit tests:
```console
make test
```
This target will compile the current branch, then run [pytest](https://docs.pytest.org/en/latest/) unit tests
against the branch. The target will also run [Valgrind](https://valgrind.org/) tests in an attempt to detect
memory management bugs.
### Formatting
Code in this project is formatted using [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html). This
project uses the **Microsoft** formatting style. To format the code, run:
```console
make format
```
### Linting
Code in this project is linted using [cppcheck](https://cppcheck.sourceforge.io/). To run the linter:
```console
make lint
```
