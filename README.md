# GPTifier
A beautiful C++ libcurl / ChatGPT interface

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/dsw7/GPTifier/master/LICENSE)
[![GitHub Releases](https://img.shields.io/github/release/dsw7/GPTifier.svg)](https://github.com/dsw7/GPTifier/releases)

## Table of Contents
- [Motivation](#motivation)
- [Setup](#setup)
  - [Prerequisites](#prerequisites)
  - [Get specific release](#get-specific-release)
  - [Compile binary](#compile-binary)
  - [Get `json.hpp`](#get-jsonhpp)
  - [Get `toml.hpp`](#get-tomlhpp)
  - [Get `{fmt}`](#get-fmt)
  - [Set up project files](#set-up-project-files)
  - [Clean up](#clean-up)
- [Usage](#usage)
  - [The `run` command](#the-run-command)
    - [Basic example](#basic-example)
    - [Exporting a result](#exporting-a-result)
    - [Specifying a model](#specifying-a-model)
  - [The `embed` command](#the-embed-command)
  - [The `models` command](#the-models-command)
- [Integrations](#integrations)
  - [Coupling with `vim`](#coupling-with-vim)
  - [GPTifier administration via OpenAI platform](#gptifier-administration-via-openai-platform)
- [Development](#development)
  - [Testing](#testing)
  - [Formatting](#formatting)
  - [Linting C++](#linting-c)
  - [Linting bash](#linting-bash)

## Motivation
There are numerous ChatGPT command line programs currently available. Many of them are written in Python. I
wanted something a bit quicker and a bit easier to install, so I wrote this program in C++.

## Setup
### Prerequisites
Ensure that you have access to a valid OpenAI API key and ensure that this API key is set as the following
environment variable:
```
OPENAI_API_KEY="<your-api-key>"
```
### Get specific release
Interested in a specific release? To download `1.0.0`, for example:
```console
wget https://github.com/dsw7/GPTifier/archive/refs/tags/v1.0.0.tar.gz
```
Which will yield:
```
v1.0.0.tar.gz
```
Then run:
```console
tar -xvf v1.0.0.tar.gz
```
Which will generate:
```
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
### Get `{fmt}`
This project uses [{fmt}](https://fmt.dev/latest/) for string formatting. The compiler will abort if `{fmt}`
cannot be found anywhere. See [Get Started](https://fmt.dev/latest/get-started/) for instructions on
installing `{fmt}`.
### Set up project files
This project makes reference to a "home directory" (`~/.gptifier`, specifically) that must be set up prior to
running the program. To set up `~/.gptifier`, run:
```console
./setup
```
This script will dump a configuration file under `~/.gptifier`. Open the file:
```console
~/.gptifier/gptifier.toml
```
And apply the relevant configurations. Next, drop into the program:
```console
gpt run
```
The program should start an interactive session if the configuration file was properly set up.
### Clean up
The compilation process will generate many build artifacts. Clean up the build artifacts by running:
```console
make clean
```

## Usage
### The `run` command
This command works with OpenAI's chat completion models, such as GPT-4 Turbo and GPT-4.
#### Basic example
Simply run `gpt run`! This will begin an interactive session. Type in a prompt:
```console
$ gpt run
------------------------------------------------------------------------------------------
Input: What is 3 + 5?
```
And hit <kbd>Enter</kbd>. The program will dispatch a request and return:
```console
...
Results: 3 + 5 equals 8.
------------------------------------------------------------------------------------------
Export:
> Write reply to file? [y/n]:
```
#### Exporting a result
In the above example, the user is prompted to export the completion a file. Entering <kbd>y</kbd> will print:
```console
...
> Writing reply to file /home/<your-username>/.gptifier/completions.gpt
------------------------------------------------------------------------------------------
```
Subsequent requests will append to this file. In some cases, prompting interactively may be undesirable, such
as when running automated unit tests. To disable the <kbd>y/n</kbd> prompt, run `gpt run` with the `-u` or
`--no-interactive-export` flags.
#### Specifying a model
A chat completion can be run against an available model by specifying the model name using the `-m` or
`--model` option. For example, to create a chat completion via command line using the GPT-4 model, run:
```console
gpt run --model gpt-4 --prompt "What is 3 + 5?"
```
A full list of models can be found by running the [models command](#the-models-command).

### The `embed` command
This command converts some input text into a vector representation of the text. To use the command, run:
```console
gpt embed
------------------------------------------------------------------------------------------
Input: Convert me to a vector!
```
And hit <kbd>Enter</kbd>. The program will dispatch a request and return:
```
------------------------------------------------------------------------------------------
Request: {
  "input": "Convert me to a vector!",
  "model": "text-embedding-ada-002"
}
...
```
The results will be exported to a JSON file: `~/.gptifier/embeddings.gpt`. In a nutshell, the `embeddings.gpt`
file will contain a vector:

$$
\begin{bmatrix}a\_1 & a\_2 & \dots & a_{1536}\end{bmatrix},
$$

Where 1536 is the dimension of the output vector corresponding to model `text-embedding-ada-002`. The cosine
similarity of a set of such vectors can be used to evaluate the similarity between text.

### The `models` command
This command returns a list of currently available models. Simply run:
```console
gpt models
```
Which will return:
```console
------------------------------------------------------------------------------------------
Model ID                      Owner                         Creation time
------------------------------------------------------------------------------------------
dall-e-3                      system                        2023-10-31 20:46:29
whisper-1                     openai-internal               2023-02-27 21:13:04
davinci-002                   system                        2023-08-21 16:11:41
...                           ...                           ...
```

## Integrations
### Coupling with `vim`
In the [Exporting a result](#exporting-a-result) section, it was stated that results can be voluntarily
exported to `~/.gptifier/completions.gpt`. One may be interested in integrating this into a `vim` workflow.
This can be achieved as follows. First, add the following function to `~/.vimrc`:
```vim
function OpenGPTifierResults()
  let l:results_file = expand('~') . '/.gptifier/completions.gpt'

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
The command `G` will open `~/.gptifier/completions.gpt` in a separate vertical split, thus allowing for cherry
picking saved OpenAI completions into a source file, for example.

### GPTifier administration via OpenAI platform
GPTifier's access to OpenAI resources can be managed by setting up a GPTifier project under [OpenAI's user
platform](https://platform.openai.com/). Some possibilities include setting usage and model limits. To
integrate GPTifier with an OpenAI project, open GPTifier's configuration file:
```console
vim +/project-id ~/.gptifier/gptifier.toml
```
And set `project-id` to the project ID associated with the newly created GPTifier project. The ID can be
obtained from the [General settings](https://platform.openai.com/settings/organization/general) page
(authentication is required).

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
### Linting C++
Code in this project is linted using [cppcheck](https://cppcheck.sourceforge.io/). To run the linter:
```console
make lint
```
### Linting bash
All bash code in this project is subjected to **shellcheck** static analysis. Run:
```console
make sc
```
See [shellcheck](https://github.com/koalaman/shellcheck) for more information.
