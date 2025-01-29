from json import loads
from os import EX_OK
from pathlib import Path
from subprocess import run
from typing import Any
from pytest import mark
from utils import unpack_stdout_stderr, EX_MEM_LEAK, Command, Capture

PROMPT = "What is 3 + 5? Format the result as follows: >>>{result}<<<"


def load_content(json_file: str) -> Any:
    with open(json_file) as f:
        contents = loads(f.read())

    return contents["choices"][0]["message"]["content"]


@mark.parametrize("option", ["-h", "--help"])
def test_run_help(command: Command, option: str, capfd: Capture) -> None:
    command.extend(["run", option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "Synopsis" in stdout


def test_read_from_command_line(
    json_file: str, command: Command, capfd: Capture
) -> None:
    command.extend(["run", f"-p'{PROMPT}'", "-t0", f"-d{json_file}", "-u"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert load_content(json_file) == ">>>8<<<"


def test_read_from_file(json_file: str, command: Command, capfd: Capture) -> None:
    prompt = Path(__file__).resolve().parent / "prompt_basic.txt"

    command.extend(["run", f"-r{prompt}", "-t0", f"-d{json_file}", "-u"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert load_content(json_file) == ">>>8<<<"


def test_read_from_inputfile(
    json_file: str, command: Command, inputfile: Path, capfd: Capture
) -> None:
    inputfile.write_text(PROMPT)

    command.extend(["run", "-t0", f"-d{json_file}", "-u"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert load_content(json_file) == ">>>8<<<"


@mark.parametrize("temp", [-2.5, 2.5])
def test_invalid_temp(command: Command, temp: float, capfd: Capture) -> None:
    command.extend(["run", f"-p'{PROMPT}'", f"-t{temp}", "-u"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert "Temperature must be between 0 and 2" in stderr
    assert process.returncode != EX_OK


def test_missing_prompt_file(command: Command, capfd: Capture) -> None:
    command.extend(["run", "--read-from-file=/tmp/yU8nnkRs.txt", "-u"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode not in (EX_OK, EX_MEM_LEAK)
    assert "Could not open file '/tmp/yU8nnkRs.txt'" in stderr


def test_invalid_dump_location(command: Command, capfd: Capture) -> None:
    command.extend(["run", f"--prompt='{PROMPT}'", "--dump=/tmp/a/b/c", "-u"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode not in (EX_OK, EX_MEM_LEAK)
    assert "Unable to open '/tmp/a/b/c'" in stderr


def test_invalid_model(command: Command, capfd: Capture) -> None:
    command.extend(["run", f"-p'{PROMPT}'", "-mfoobar", "-u"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert (
        "The model `foobar` does not exist or you do not have access to it." in stderr
    )
    assert process.returncode != EX_OK
