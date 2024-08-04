from json import loads
from os import EX_OK
from pathlib import Path
from subprocess import run
from pytest import mark
from utils import unpack_stdout_stderr, EX_MEM_LEAK

PROMPT = "What is 3 + 5? Format the result as follows: >>>{result}<<<"


def load_content(json_file: str) -> str:
    with open(json_file) as f:
        contents = loads(f.read())

    return contents["choices"][0]["message"]["content"]


def load_error(json_file: str) -> str:
    with open(json_file) as f:
        contents = loads(f.read())

    return contents["error"]["message"]


@mark.parametrize("option", ["-h", "--help"])
def test_run_help(command: list[str], option: str, capfd) -> None:
    command.extend(["run", option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "SYNOPSIS" in stdout


def test_read_from_command_line(json_file: str, command: list[str], capfd) -> None:
    command.extend(["run", f"-p'{PROMPT}'", "-t0", f"-d{json_file}", "-u"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert load_content(json_file) == ">>>8<<<"


def test_read_from_file(json_file: str, command: list[str], capfd) -> None:
    prompt = Path(__file__).resolve().parent / "prompt_basic.txt"

    command.extend(["run", f"-r{prompt}", "-t0", f"-d{json_file}", "-u"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert load_content(json_file) == ">>>8<<<"


def test_read_from_inputfile(
    json_file: str, command: list[str], inputfile: Path, capfd
) -> None:
    inputfile.write_text(PROMPT)

    command.extend(["run", "-t0", f"-d{json_file}", "-u"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert load_content(json_file) == ">>>8<<<"


MESSAGES_BAD_TEMP = [
    (-2.5, "decimal below minimum value. Expected a value >= 0, but got -2.5 instead."),
    (2.5, "decimal above maximum value. Expected a value <= 2, but got 2.5 instead."),
]


@mark.parametrize("temp, message", MESSAGES_BAD_TEMP)
def test_invalid_temp(
    json_file: str, command: list[str], temp: float, message: str, capfd
) -> None:
    command.extend(["run", f"-p'{PROMPT}'", f"-t{temp}", f"-d{json_file}", "-u"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert load_error(json_file) == f"Invalid 'temperature': {message}"


def test_missing_prompt_file(command: list[str], capfd) -> None:
    command.extend(["run", "--read-from-file=/tmp/yU8nnkRs.txt", "-u"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode not in (EX_OK, EX_MEM_LEAK)
    assert "Could not open file '/tmp/yU8nnkRs.txt'" in stderr


def test_invalid_dump_location(command: list[str], capfd) -> None:
    command.extend(["run", f"--prompt='{PROMPT}'", "--dump=/tmp/a/b/c", "-u"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode not in (EX_OK, EX_MEM_LEAK)
    assert "Unable to open '/tmp/a/b/c'" in stderr


def test_invalid_model(json_file: str, command: list[str], capfd) -> None:
    command.extend(["run", f"-p'{PROMPT}'", "-mfoobar", f"-d{json_file}", "-u"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert (
        load_error(json_file)
        == "The model `foobar` does not exist or you do not have access to it."
    )
