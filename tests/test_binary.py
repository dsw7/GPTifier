from json import loads
from pathlib import Path
from subprocess import run, CalledProcessError, PIPE, DEVNULL
from tempfile import TemporaryDirectory
from typing import Generator
import pytest


@pytest.fixture
def tempdir() -> Generator[Path, None, None]:
    t_dir = TemporaryDirectory()
    yield Path(t_dir.name)
    t_dir.cleanup()


def test_basic(tempdir: Path) -> None:
    json_file = tempdir / "test_basic.json"

    command = [
        "build/gpt",
        "--prompt='What is 3 + 5? Format the result as follows: >>>{result}<<<'",
        "--temperature=0",
        f"--dump={json_file}",
    ]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())

    with json_file.open() as f:
        data = loads(f.read())
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

    command = [
        "build/gpt",
        "--prompt='Running a test!'",
        f"--dump={json_file}",
        f"--temperature={temp}",
    ]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())

    with json_file.open() as f:
        data = loads(f.read())
        assert data["error"]["message"] == result


def test_read_from_file(tempdir: Path) -> None:
    json_file = tempdir / "test_read_from_file.json"

    prompt = tempdir / "test_read_from_file.txt"
    prompt.write_text("What is 3 + 5? Format the result as follows: >>>{result}<<<")

    command = [
        "build/gpt",
        f"--read-from-file={prompt}",
        "--temperature=0",
        f"--dump={json_file}",
    ]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())

    with json_file.open() as f:
        data = loads(f.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"


def test_missing_file() -> None:
    command = ["build/gpt", "--read-from-file=/tmp/yU8nnkRs.txt"]

    with pytest.raises(CalledProcessError):
        run(command, stdout=DEVNULL, stderr=DEVNULL, check=True)
