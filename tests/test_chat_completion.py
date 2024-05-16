from json import loads
from os import EX_OK
from pathlib import Path
from subprocess import run, DEVNULL, PIPE, CalledProcessError
from pytest import fail, mark
from consts import EX_MEM_LEAK
from utils import print_stdout, print_stderr


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
    output = capfd.readouterr()
    print()

    print_stdout(output.out)
    print_stderr(output.err)
    assert process.returncode == EX_OK

    with open(json_file) as f_json:
        data = loads(f_json.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"


@mark.parametrize(
    "temp, result",
    [
        (
            "-2.5",
            "Invalid 'temperature': decimal below minimum value. Expected a value >= 0, but got -2.5 instead.",
        ),
        (
            "2.5",
            "Invalid 'temperature': decimal above maximum value. Expected a value <= 2, but got 2.5 instead.",
        ),
    ],
)
def test_invalid_temp(
    temp: str, result: str, json_file: str, command: list[str]
) -> None:
    command.extend(["run", "-p'Running a test!'", f"-t{temp}", f"-d{json_file}", "-u"])

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())

    with open(json_file) as f_json:
        data = loads(f_json.read())
        assert data["error"]["message"] == result


def test_read_from_file(json_file: str, command: list[str]) -> None:
    prompt = Path(__file__).resolve().parent / "prompt_basic.txt"
    command.extend(["run", f"-r{prompt}", "-t0", f"-d{json_file}", "-u"])

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())

    with open(json_file) as f_json:
        data = loads(f_json.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"


def test_missing_prompt_file(command: list[str], capfd) -> None:
    command.extend(["run", "--read-from-file=/tmp/yU8nnkRs.txt", "-u"])

    process = run(command, stdout=DEVNULL)
    assert process.returncode not in (EX_OK, EX_MEM_LEAK)

    cap = capfd.readouterr()
    assert "Could not open file '/tmp/yU8nnkRs.txt'" in cap.err


def test_invalid_dump_loc(command: list[str], capfd) -> None:
    command.extend(["run", "--prompt='What is 3 + 5?'", "--dump=/tmp/a/b/c", "-u"])

    process = run(command, stdout=DEVNULL)
    assert process.returncode not in (EX_OK, EX_MEM_LEAK)

    cap = capfd.readouterr()
    assert "Unable to open '/tmp/a/b/c'" in cap.err


@mark.parametrize(
    "model, result, valid_model",
    [
        ("gpt-3.5-turbo-0301", "gpt-3.5-turbo-0301", True),
        (
            "gpt-3.5-turbo-0302",
            "The model `gpt-3.5-turbo-0302` does not exist or you do not have access to it.",
            False,
        ),
    ],
)
def test_model(
    model: str, result: str, valid_model: bool, json_file: str, command: list[str]
) -> None:
    command.extend(["run", "-p'What is 3 + 5?'", f"-m{model}", f"-d{json_file}", "-u"])

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())

    with open(json_file) as f_json:
        data = loads(f_json.read())

    if valid_model:
        assert data["model"] == result
    else:
        assert data["error"]["message"] == result


def test_run_help(command: list[str], capfd) -> None:
    command.extend(["run", "--help"])

    process = run(command)
    assert process.returncode == EX_OK

    cap = capfd.readouterr()
    assert "SYNOPSIS" in cap.out
