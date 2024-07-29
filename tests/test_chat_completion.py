from json import loads
from os import EX_OK
from pathlib import Path
from subprocess import run
from pytest import mark
import utils


def test_basic(json_file: str, command: list[str], capfd) -> None:
    command.extend(
        [
            "run",
            "-p'What is 3 + 5? Format the result as follows: >>>{result}<<<'",
            "-t0",
            f"-d{json_file}",
            "-u",
        ]
    )
    process = run(command)

    utils.print_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    with open(json_file) as f_json:
        data = loads(f_json.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"


MESSAGES_BAD_TEMP = [
    (-2.5, "decimal below minimum value. Expected a value >= 0, but got -2.5 instead."),
    (2.5, "decimal above maximum value. Expected a value <= 2, but got 2.5 instead."),
]


@mark.parametrize("temp, message", MESSAGES_BAD_TEMP)
def test_invalid_temp(
    json_file: str, command: list[str], temp: float, message: str, capfd
) -> None:
    command.extend(["run", "-p'Running a test!'", f"-t{temp}", f"-d{json_file}", "-u"])
    process = run(command)

    utils.print_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    with open(json_file) as f_json:
        data = loads(f_json.read())
        assert data["error"]["message"] == "Invalid 'temperature': " + message


def test_read_from_file(json_file: str, command: list[str], capfd) -> None:
    prompt = Path(__file__).resolve().parent / "prompt_basic.txt"

    command.extend(["run", f"-r{prompt}", "-t0", f"-d{json_file}", "-u"])
    process = run(command)

    utils.print_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    with open(json_file) as f_json:
        data = loads(f_json.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"


def test_missing_prompt_file(command: list[str], capfd) -> None:
    command.extend(["run", "--read-from-file=/tmp/yU8nnkRs.txt", "-u"])
    process = run(command)

    capture = capfd.readouterr()
    print()
    utils.print_stdout(capture.out)
    utils.print_stderr(capture.err)

    assert process.returncode not in (EX_OK, utils.EX_MEM_LEAK)
    assert "Could not open file '/tmp/yU8nnkRs.txt'" in capture.err


def test_invalid_dump_location(command: list[str], capfd) -> None:
    command.extend(["run", "--prompt='What is 3 + 5?'", "--dump=/tmp/a/b/c", "-u"])
    process = run(command)

    capture = capfd.readouterr()
    print()
    utils.print_stdout(capture.out)
    utils.print_stderr(capture.err)

    assert process.returncode not in (EX_OK, utils.EX_MEM_LEAK)
    assert "Unable to open '/tmp/a/b/c'" in capture.err


def test_invalid_model(json_file: str, command: list[str], capfd) -> None:
    command.extend(["run", "-p'What is 3 + 5?'", "-mfoobar", f"-d{json_file}", "-u"])
    process = run(command)

    utils.print_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    with open(json_file) as f_json:
        data = loads(f_json.read())
        assert (
            data["error"]["message"]
            == "The model `foobar` does not exist or you do not have access to it."
        )


def test_run_help(command: list[str], capfd) -> None:
    command.extend(["run", "--help"])
    process = run(command)

    capture = capfd.readouterr()
    print()
    utils.print_stdout(capture.out)
    utils.print_stderr(capture.err)

    assert process.returncode == EX_OK
    assert "SYNOPSIS" in capture.out
