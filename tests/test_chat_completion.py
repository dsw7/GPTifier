from json import loads
from pathlib import Path
import subprocess
import pytest


def test_basic(tempdir: Path) -> None:
    json_file = tempdir / "test_basic.json"

    command = [
        "build/gpt",
        "-u",
        "-p'What is 3 + 5? Format the result as follows: >>>{result}<<<'",
        "-t0",
        f"-d{json_file}",
    ]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())

    with json_file.open() as f_json:
        data = loads(f_json.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"


@pytest.mark.parametrize(
    "temp, result",
    [
        ("-2.5", "-2.5 is less than the minimum of 0 - 'temperature'"),
        ("2.5", "2.5 is greater than the maximum of 2 - 'temperature'"),
    ],
)
def test_invalid_temp(temp: str, result: str, tempdir: Path) -> None:
    json_file = tempdir / "test_invalid_temp.json"

    command = ["build/gpt", "-u", "-p'Running a test!'", f"-d{json_file}", f"-t{temp}"]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())

    with json_file.open() as f_json:
        data = loads(f_json.read())
        assert data["error"]["message"] == result


def test_read_from_file(tempdir: Path) -> None:
    json_file = tempdir / "test_read_from_file.json"

    prompt = tempdir / "test_read_from_file.txt"
    prompt.write_text("What is 3 + 5? Format the result as follows: >>>{result}<<<")

    command = ["build/gpt", "-u", f"-r{prompt}", "-t0", f"-d{json_file}"]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())

    with json_file.open() as f_json:
        data = loads(f_json.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"


def test_missing_file() -> None:
    command = ["build/gpt", "-r/tmp/yU8nnkRs.txt"]

    with pytest.raises(subprocess.CalledProcessError):
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True
        )


@pytest.mark.parametrize(
    "model, result, valid_model",
    [
        ("gpt-3.5-turbo-0301", "gpt-3.5-turbo-0301", True),
        ("gpt-3.5-turbo-0302", "The model `gpt-3.5-turbo-0302` does not exist", False),
    ],
)
def test_model(model: str, result: str, valid_model: bool, tempdir: Path) -> None:
    json_file = tempdir / "test_model.json"

    command = ["build/gpt", "-u", "-p'What is 3 + 5?'", f"-m{model}", f"-d{json_file}"]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())

    with json_file.open() as f_json:
        data = loads(f_json.read())

    if valid_model:
        assert data["model"] == result
    else:
        assert data["error"]["message"] == result
