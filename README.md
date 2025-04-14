# GPTifier

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/dsw7/GPTifier/master/LICENSE)
[![GitHub Releases](https://img.shields.io/github/release/dsw7/GPTifier.svg)](https://github.com/dsw7/GPTifier/releases)

## Overview
**GPTifier** is a command-line tool designed to interact seamlessly with OpenAI's API. I designed this program
to "look and feel" similar to `git`. I wrote this program in C++ to enhance performance and responsiveness,
resulting in a faster and more efficient user experience.

## Table of Contents
- [What can GPTifier do?](#what-can-gptifier-do)
- [Installation](#installation)
- [Usage](#usage)
  - [The `run` command](#the-run-command)
  - [The `short` command](#the-short-command)
  - [The `embed` command](#the-embed-command)
  - [The `models` command](#the-models-command)
  - [The `files` command](#the-files-command)
  - [The `fine-tune` command](#the-fine-tune-command)
  - [The `chats` command](#the-chats-command)
  - [The `edit` command](#the-edit-command)
- [Administration](#administration)
  - [The `costs` command](#the-costs-command)
- [Integrations](#integrations)
- [Use cases](#use-cases)
  - [Using GPTifier to maximize token efficiency](#using-gptifier-to-maximize-token-efficiency)

## What can GPTifier do?
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
Any new responses will append to this file. To bypass the save-file prompt during operations like automated
tests, use the `-u` or `--no-interactive-export` flags.

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

#### Storing prompt/completion pairs
Use the `-s` or `--store-completion` flag to temporarily store prompt/completion pairs on OpenAI servers.
Retrieve them later with the [chats command](#the-chats-command).

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

### The `chats` command
The `chats` family of subcommands is used to manage stored chat completions, such as those saved using the
`--store-completion` flag with the `gpt run` command. To list stored chat completions, you can run:
```console
gpt chats
# or
gpt chats list
```
To delete a specific chat completion, use the following command:
```console
gpt chats delete <chat-completion-id>
```
Here, `<chat-completion-id>` is the identifier obtained from `gpt chats`, corresponding to the chat completion
you wish to delete.

### The `edit` command
The `edit` command allows users to modify files according to instructions specified in a separate text file.
For instance, if `foo.cpp` uses CamelCase formatting and needs to be converted to snake_case, you can outline
the necessary changes in an instruction file, such as `prompt.txt`:
```plaintext
Convert all code from CamelCase to snake_case.
```
To display the results in the terminal, use the following command:
```console
gpt edit foo.cpp -p prompt.txt
```
To save the updated code to a new file, such as `bar.cpp`, execute:
```console
gpt edit foo.cpp -p prompt.txt -o bar.cpp
```
If you wish to overwrite the original file, simply run:
```console
gpt edit foo.cpp -p prompt.txt -o foo.cpp
```
> [!NOTE]
> The instructions in `prompt.txt` do not require prompt engineering. When processed, these instructions
> are combined with additional context and output format specifications to create a complete prompt.

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
