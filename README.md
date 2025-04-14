# GPTifier

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/dsw7/GPTifier/master/LICENSE)
[![GitHub Releases](https://img.shields.io/github/release/dsw7/GPTifier.svg)](https://github.com/dsw7/GPTifier/releases)

## Overview
**GPTifier** is a command-line tool designed to interact seamlessly with OpenAI's API. I designed this program
to "look and feel" similar to `git`. I wrote this program in C++ to enhance performance and responsiveness,
resulting in a faster and more efficient user experience.

## Table of Contents
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
  - [The `run` command](#the-run-command)
    - [Basic example](#basic-example)
    - [Exporting a result](#exporting-a-result)
    - [Specifying a model](#specifying-a-model)
  - [The `short` command](#the-short-command)
  - [The `embed` command](#the-embed-command)
  - [The `models` command](#the-models-command)
  - [The `files` command](#the-files-command)
    - [List files](#list-files)
    - [Delete files](#delete-files)
  - [The `fine-tune` command](#the-fine-tune-command)
    - [Fine tuning workflow](#fine-tuning-workflow)
  - [The `chats` command](#the-chats-command)
  - [The `edit` command](#the-edit-command)
  - [Input selection](#input-selection)
- [Administration](#administration)
  - [The `costs` command](#the-costs-command)
- [Integrations](#integrations)
  - [Coupling with `vim`](#coupling-with-vim)
  - [GPTifier administration via OpenAI platform](#gptifier-administration-via-openai-platform)
- [Use cases](#use-cases)
  - [Using GPTifier to maximize token efficiency](#using-gptifier-to-maximize-token-efficiency)

## Features
- **Run Queries**: Execute prompts and receive completions via the CLI, similar to querying ChatGPT.
- **Retrieve Models**: List the latest OpenAI models and custom models.
- **Edit Files**: Modify individual files with an approach akin to `sed`, but with enhanced intelligence.
- **Create Embeddings**: Generate and export vector embeddings.
- **Fine-Tuning Management**: Fully manage fine-tuning workflows directly from the command line.
- **Additional Tasks**: Handle uploaded files, monitor usage, and perform various other tasks.

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
Install [{fmt}](https://fmt.dev/latest/) for string formatting. If `{fmt}` is not found, the compiler will
abort. Install it via:
```bash
apt install libfmt-dev
```
Alternatively, refer to the [{fmt} Get Started guide](https://fmt.dev/latest/get-started/) for other
installation methods.

### Step 1: Compile the binary
Compile the binary by executing the `make` target:
```bash
make compile
```
The binary will be installed into the directory specified by CMake's [install()
function](https://cmake.org/cmake/help/latest/command/install.html#command:install). To clean up generated
artifacts:
```bash
make clean
```

### Step 2: Run the setup script
This project requires a specific "project directory" (`~/.gptifier`). Set it up by running:
```bash
./setup
```

### Step 3: Edit configurations
The setup script generates a configuration file at `~/.gptifier/gptifier.toml`. Open this file and adjust the
configurations accordingly. 

Next, start the program:
```bash
gpt run
```
The program will initiate an interactive session if the configuration file is set up correctly.

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

> [!TIP]
> A full list of models can be found by running the [models command](#the-models-command)

> [!NOTE]
> See [Input selection](#input-selection) for more information regarding how to pass
> a prompt into this command

### The `short` command
The `short` command is almost identical to the [the `run` command](#the-run-command), but this command returns
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

> [!NOTE]
> See [Input selection](#input-selection) for more information regarding how to pass
> embedding text into this command

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

### The `files` command
Use this command to manage files uploaded to OpenAI.
#### List files
To list uploaded files, run:
```console
gpt files
# or
gpt files list
```
#### Delete files
To delete one or more uploaded files, run:
```console
gpt files delete <file-id>
```
The file ID corresponding to a file can be obtained by running [the `list` subcommand](#list-files).

### The `fine-tune` command
The `fine-tune` command can be used for managing fine-tuning operations.
#### Fine tuning workflow
Start off by creating a dataset. See [Preparing your
dataset](https://platform.openai.com/docs/guides/fine-tuning#preparing-your-dataset) for more information.
Next, upload the dataset:
```console
gpt fine-tune upload-file jessica_training.jsonl # jessica_training.jsonl is the dataset
```
Which will print out:
```
Success!
Uploaded file: jessica_training.jsonl
With ID: file-6Vf...8t7
```
Next, create a fine-tuning job:
```console
gpt fine-tune create-job --file-id=file-6Vf...8t7 --model=gpt-4o-mini-2024-07-18
```
Then check the status of the job:
```console
gpt fine-tune list-jobs
```
If the training file is no longer needed, the file can be deleted from OpenAI servers:
```console
gpt files delete file-6Vf...8t7
```
And if the model is no longer needed, the model can be deleted using:
```console
gpt fine-tune delete-model <model-id>
```
The model ID can be found from the `User models` section listed by running:
```console
gpt models
```

### The `chats` command
The `chats` family of subcommands can be used to manage stored chat completions (for example, chat completions
that were saved with the `--store-completion` flag when using `gpt run`). Run:
```console
gpt chats
# or
gpt chats list
```
To list stored completions. A chat completion can be deleted by running:
```console
gpt chats delete <chat-completion-id>
```
Where `<chat-completion-id>` is collected from `gpt chats` and corresponds to a chat completion that is to be
deleted.

### The `edit` command
Use this command to edit files according to instructions written into a text file. For example, suppose
`foo.cpp` contains Camel case formatted code, and suppose there's a need to change the format to snake case.
Instructions for applying this change can be written into a prompt file, say `prompt.txt`:
```plaintext
Convert all code from camel case to snake case.
```
The following command will print the results to `stdout`:
```console
gpt edit foo.cpp -p prompt.txt
```
To write the updated code to a new file, say `bar.cpp`:
```console
gpt edit foo.cpp -p prompt.txt -o bar.cpp
```
And to simply overwrite `foo.cpp`, run:
```console
gpt edit foo.cpp -p prompt.txt -o foo.cpp
```
> [!NOTE]
> No prompt engineering is needed when writing `prompt.txt`. The instructions written to `prompt.txt` are
> concatenated into a larger prompt complete with context, output format specifications, etc.

### Input selection
For certain commands, a hierarchy exists for choosing where input text comes from. The hierarchy roughly
follows:

1. **Check for raw input via command line option**:
   - If raw input is provided through a command line option, use this input
   - Example: `gpt run -p "What is 3 + 5?"` or `gpt embed -i "A foo that bars"`

2. **Check for input file specified via command line**:
   - If a file path is provided as a command line argument, read from this file
   - Example: `gpt [run | embed] -r <filename>`

3. **Check for a default input file in the current working directory**:
   - If a file named `Inputfile` exists in the current directory, read from this file
   - This is analogous to a `Makefile` or perhaps a `Dockerfile`

4. **Read from stdin**:
   - If none of the above conditions are met, read input from standard input (stdin)

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

## Use cases
### Using GPTifier to maximize token efficiency
GPTifier can be used to study prompt quality in an attempt to optimize for token usage. For example, passing
a very short prompt:
```console
gpt run -p "Running a test"
```
Will return:
```
...
Usage:
Prompt tokens: 10
Prompt size (words): 3
Ratio: 3.3333333

Completion tokens: 17
Completion size (words): 14
Ratio: 1.2142857
```
However, passing a slightly more descriptive prompt:
```console
gpt run -p "I am running a small test. Please ignore me!"
```
Returns:
```
...
Usage:
Prompt tokens: 18
Prompt size (words): 9
Ratio: 2

Completion tokens: 16
Completion size (words): 13
Ratio: 1.2307693
```
Notice that in this case, slightly more tokens are being used, but the token-to-word ratio approaches the
"typical" 100 tokens / 75 words ratio cited by OpenAI. This output can also be used to modulate the completion
token usage. For example, the prompt:
```console
gpt run -p "A foo that bars"
```
Will return:
```
Results: Sounds like a playful twist on terminology! In programming, ...
...
Prompt tokens: 11
Prompt size (words): 4
Ratio: 2.75

Completion tokens: 87
Completion size (words): 66
Ratio: 1.3181819
...
```
However, tweaking the prompt:
```console
gpt run -p "A foo that bars. Short answer please"
```
Will return:
```
Results: A playful expression often used to describe ...
...
Prompt tokens: 15
Prompt size (words): 7
Ratio: 2.142857

Completion tokens: 34
Completion size (words): 27
Ratio: 1.2592592
...
```
In this case, 4 extra prompt tokens were used corresponding to the request "Short answer please", however,
this brought down the number of completion tokens from 87 to 34.
