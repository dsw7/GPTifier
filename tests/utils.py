from os import environ
from json import loads
from typing import Any
from subprocess import run, PIPE


def load_stdout_to_json(stdout: str) -> Any:
    return loads(stdout)


def assert_command_success(*args: str) -> str:
    command = [environ["PATH_BIN"]]
    command.extend(args)
    process = run(command, stdout=PIPE, stderr=PIPE, text=True)
    assert process.returncode == 0, process.stderr
    return process.stdout


def assert_command_failure(*args: str) -> str:
    command = [environ["PATH_BIN"]]
    command.extend(args)
    process = run(command, stdout=PIPE, stderr=PIPE, text=True)
    assert process.returncode == 1
    return process.stderr
