# GPTifier

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/dsw7/GPTifier/master/LICENSE)
[![GitHub Releases](https://img.shields.io/github/release/dsw7/GPTifier.svg)](https://github.com/dsw7/GPTifier/releases)

## Overview
**GPTifier** is a command line tool designed to interact with OpenAI's API. The
program supports a few operations out of the box, such as prompting,
fine-tuning, generating images, etc. This program is written purely in C++
because python just feels clunky and slow. This program is tested on
Ubuntu/Debian and macOS.

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
  - [The `run` command](#the-run-command)
  - [The `short` command](#the-short-command)
  - [The `embed` command](#the-embed-command)
  - [The `models` command](#the-models-command)
  - [The `files` command](#the-files-command)
  - [The `fine-tune` command](#the-fine-tune-command)
  - [The `img` command](#the-img-command)
- [Administration](#administration)
  - [The `costs` command](#the-costs-command)
- [Code editing](#code-editing)
- [Integrations](#integrations)
- [Uninstall GPTifier](#uninstall-gptifier)
- [License](#license)

## Installation

### Prerequisites
Ensure you possess a valid OpenAI API key. Set it as an environment variable:
```bash
export OPENAI_API_KEY="<your-api-key>"
```
Set an additional administrative key as an environment variable for running administrative commands:
```bash
export OPENAI_ADMIN_KEY="<your-admin-key>"
```
This program requires [CMake](https://cmake.org/), [{fmt}](https://fmt.dev/latest/) and
[libcurl](https://curl.se/libcurl/). These can be installed as follows:

#### Ubuntu/Debian
```console
apt install cmake libfmt-dev libcurl4-openssl-dev
```
#### macOS
```console
brew install cmake fmt
# libcurl usually comes bundled with macOS
```
#### Other systems
This program should work on other Unix-like systems (i.e. other Linux distributions) however I do not
extensively test these.

### Step 1: Compile the binary
Compile the binary by executing the `make` target:
```console
make
```
The binary will be installed into the directory specified by CMake's [install()
function](https://cmake.org/cmake/help/latest/command/install.html#command:install). To clean up generated
artifacts:
```console
make clean
```

#### Using external libraries
This project uses [toml++](https://marzer.github.io/tomlplusplus/) and
[json](https://github.com/nlohmann/json). These header files will be downloaded from their respective
repositories by default. To instead specify paths to external header files, the project can instead be
compiled with definitions, for example:
```console
# i.e. use a custom toml.hpp under /tmp
cmake -DUSE_SYSTEM_TOMLPLUSPLUS=ON -DTOMLPLUSPLUS_HPP=/tmp/toml.hpp -S GPTifier -B /tmp/build && make -j12 -C /tmp/build install
```
And:
```console
# i.e. use a custom json.hpp under /tmp
cmake -DUSE_SYSTEM_NLOHMANN_JSON=ON -DNLOHMANN_JSON_HPP=/tmp/json.hpp -S GPTifier -B /tmp/build && make -j12 -C /tmp/build install
```

### Step 2: Run the setup script
This project requires a specific "project directory" (`~/.gptifier`). Set it up by running:
```console
./setup
```

### Step 3: Edit configurations
The setup script generates a configuration file at `~/.gptifier/gptifier.toml`. Open this file and adjust the
configurations accordingly.

Next, start the program:
```console
gpt run
```
The program will initiate an interactive session if the configuration file is set up correctly. You may get
some variation of:
```
-bash: gpt: command not found
```
If so, try running `gpt` in a new terminal window.

## Usage

### The `run` command
The `run` command allows you to query OpenAI models like GPT-4. To start an interactive session, enter:
```console
gpt run
```
You'll see a prompt where you can type your query:
```
------------------------------------------------------------------------------------------
Input: What is 3 + 5?
```
The program processes the request and returns the answer:
```console
...
Results: 3 + 5 equals 8.
------------------------------------------------------------------------------------------
Export:
> Write reply to file? [y/n]:
```
You will be asked if you want to save the response to a file. If you choose <kbd>y</kbd>, the output will be
saved:
```console
...
> Writing reply to file /home/<your-username>/.gptifier/completions.gpt
------------------------------------------------------------------------------------------
```
Any new responses will append to this file.

#### Specifying a model
To specify a model for chat completion, use the `-m` or `--model` option. For example, to use GPT-4:
```console
gpt run --model gpt-4 --prompt "What is 3 + 5?"
```

> [!TIP]
> To see all available models, use the [models command](#the-models-command).

#### Handling long, multiline prompts
For multiline prompts, create a file named `Inputfile` in your working directory. GPTifier will automatically
read from it. Alternatively, use the `-r` or `--read-from-file` option to specify a custom file.

### The `short` command
The `short` command is almost identical to the [run command](#the-run-command), but this command returns
a chat completion under the following conditions:

1. Threading is disabled; that is, no timer will run in the background to time the round trip
2. Verbosity is disabled; either the raw chat completion or an error will be printed to the console
3. Text colorization is disabled; this is to prevent ANSI escape code artifact clutter

An example follows:
```console
gpt short "What is 2 + 2?"
```
Which will print out:
```
2 + 2 equals 4.
```

> [!TIP]
> Use this command if running GPTifier via something like `vim`'s `system()` function

### The `embed` Command
The `embed` command converts input text into a vector representation. To embed text, execute the following:
```console
gpt embed
```
You will then be prompted with:
```
------------------------------------------------------------------------------------------
Input text to embed:
```
Enter the text you wish to embed:
```
------------------------------------------------------------------------------------------
Input text to embed: Convert me to a vector!
```
Press <kbd>Enter</kbd> to proceed. The program will generate the embedding, and the results will be saved to a
JSON file located at `~/.gptifier/embeddings.gpt`.

#### Embedding large text blocks
For large blocks of text, you can read from a file:
```console
gpt embed -r my_text.txt -o my_embedding.json # and export embedding to a custom file!
```

### The `models` command
This command returns a list of currently available models. Simply run:
```console
gpt models
```
Which will return:
```
------------------------------------------------------------------------------------------
Model ID                      Owner                         Creation time
------------------------------------------------------------------------------------------
dall-e-3                      system                        2023-10-31 20:46:29
whisper-1                     openai-internal               2023-02-27 21:13:04
davinci-002                   system                        2023-08-21 16:11:41
...                           ...                           ...
```

#### User models
User models (say fine-tuned models) can be selectively listed by passing the `-u` or `--user` flag.

### The `files` command
This command is used to manage files uploaded to OpenAI.

#### List files
To list the uploaded files, use:
```console
gpt files
# or
gpt files list
```

#### Delete files
To delete one or more uploaded files, use:
```console
gpt files delete <file-id>
```
You can obtain the file ID by running the [list subcommand](#list-files).

### The `fine-tune` command
The `fine-tune` command is used for managing fine-tuning operations.

#### Fine-tuning workflow
1. **Create a dataset:** Begin by creating a dataset. Refer to [Preparing your
   dataset](https://platform.openai.com/docs/guides/fine-tuning#preparing-your-dataset) for detailed
   instructions.

2. **Upload the dataset:**
    ```console
    gpt fine-tune upload-file jessica_training.jsonl
    ```
   Here, `jessica_training.jsonl` is the name of your dataset file. Upon successful upload, you should see a
   confirmation message similar to the following:
    ```
    Success!
    Uploaded file: jessica_training.jsonl
    With ID: file-6Vf...8t7
    ```

3. **Create a fine-tuning job:**
    ```console
    gpt fine-tune create-job --file-id=file-6Vf...8t7 --model=gpt-4o-mini-2024-07-18
    ```

4. **Check the status of the job:**
    ```console
    gpt fine-tune list-jobs
    ```

5. **Delete unneeded files:**
   If you no longer need the training file, you can delete it from the OpenAI servers:
    ```console
    gpt files delete file-6Vf...8t7
    ```

6. **Delete unneeded models:**
   If the fine-tuned model is no longer required, you can delete it using:
    ```console
    gpt fine-tune delete-model <model-id>
    ```
   To find the model ID, run:
    ```console
    gpt models -u
    ```

### The `img` command
The `img` command allows users to generate PNG images according to instructions provided in a text file. At
present time, this command only supports the use of `dall-e-3` for image generation. To generate an image,
run:
```console
gpt img /tmp/prompt.txt  # prompt.txt contains a description of the image
```

## Administration
> [!NOTE]
> The commands in this section assume that a valid `OPENAI_ADMIN_KEY` is set as an environment variable.

### The `costs` command
The `costs` command can be used to determine overall monetary usage on OpenAI resources over a specified
number of days for an organization. For example:
```console
gpt costs --days=5
```
Will return the usage per day over the past 5 days and the overall usage over the 5 days.

## Code editing
See the related project, [FuncGraft](https://github.com/dsw7/FuncGraft). This
project offers an interface similar to GPTifier, but the project is focused on
targeted code manipulations using LLMs.

## Integrations

### Integrating `vim` with `GPTifier`
In the [run command](#the-run-command) section, we discussed how completions can be optionally exported to
`~/.gptifier/completions.gpt`. If you wish to integrate access to these completions into your `vim` workflow,
you can do so by first adding the function below to your `~/.vimrc` file:
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
This function checks if the results file exists, and if so, opens it in a vertical split within `vim`. Define
a custom command in your `~/.vimrc`:
```vim
" Open GPTifier results file
command G :call OpenGPTifierResults()
```
With this command configured, you can use `:G` in `vim` to open the `~/.gptifier/completions.gpt` file in a
separate vertical split. This setup allows for easy access and selective copying of saved OpenAI completions
into your code or text files.

## Uninstall GPTifier

### Step 1: Remove binary
First, verify that the `gpt` command in your `$PATH` is the GPTifier binary and not an alias for another
application:
```console
gpt -h
```
If confirmed, proceed to remove the binary with the following command:
```console
rm $(which gpt)
```

### Step 2: Remove GPTifier directory
Before deletion, check the `~/.gptifier` directory for any files you wish to retain, such as completions or
configurations. Once reviewed, remove the directory:
```console
rm -r ~/.gptifier
```

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
