from json import loads
from os import EX_OK
from pathlib import Path
from subprocess import run
from typing import Any
from unittest import TestCase
from tempfile import NamedTemporaryFile, gettempdir

from pytest import mark
from helpers import run_process
from utils import unpack_stdout_stderr, EX_MEM_LEAK, Command, Capture

PROMPT = "What is 3 + 5? Format the result as follows: >>>{result}<<<"


def load_content(json_file: str) -> Any:
    with open(json_file) as f:
        contents = loads(f.read())

    return contents["choices"][0]["message"]["content"]


class TestChatCompletionReadFromInputfile(TestCase):
    def setUp(self) -> None:
        self.filename = Path.cwd() / "Inputfile"
        self.filename.write_text(PROMPT)

    def tearDown(self) -> None:
        self.filename.unlink()

    def test_read_from_inputfile(self) -> None:
        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            proc = run_process(["run", "-t0", f"-d{json_file}", "-u"])
            proc.assert_success()
            self.assertEqual(load_content(json_file), ">>>8<<<")


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


@mark.parametrize("temp", [-2.5, 2.5])
def test_out_of_range_temp(command: Command, temp: float, capfd: Capture) -> None:
    command.extend(["run", f"-p'{PROMPT}'", f"-t{temp}", "-u"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert "Temperature must be between 0 and 2" in stderr
    assert process.returncode != EX_OK


def test_invalid_temp(command: Command, capfd: Capture) -> None:
    command.extend(["run", f"-p'{PROMPT}'", "-tfoobar", "-u"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert "Failed to convert 'foobar' to float" in stderr
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
