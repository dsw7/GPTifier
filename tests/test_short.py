from os import EX_OK
from subprocess import run
from pytest import mark
from utils import unpack_stdout_stderr, Command, Capture


@mark.parametrize("option", ["-h", "--help"])
def test_short_help(command: Command, option: str, capfd: Capture) -> None:
    command.extend(["short", option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "SYNOPSIS" in stdout


@mark.parametrize("option", ["-p", "--prompt="])
def test_short_prompt(command: Command, option: str, capfd: Capture) -> None:
    prompt = '"What is 2 + 2? Format the result as follows: >>>{result}<<<"'
    command.extend(["short", option + prompt])

    process = run(command)
    stdout, _ = unpack_stdout_stderr(capfd)

    assert process.returncode == EX_OK
    assert ">>>4<<<" in stdout


def test_missing_prompt(command: Command, capfd: Capture) -> None:
    command.extend(["short"])

    process = run(command)
    _, stderr = unpack_stdout_stderr(capfd)

    assert process.returncode != EX_OK
    assert "Prompt is empty" in stderr
