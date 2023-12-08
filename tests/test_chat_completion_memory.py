from pathlib import Path
import subprocess
import pytest

VALGRIND_ROOT = [
    "valgrind",
    "--error-exitcode=1",
    "--leak-check=full",
    "build/gpt",
]


def test_basic_memory(tempdir: Path) -> None:
    command = VALGRIND_ROOT + [
        "-u",
        "-p'What is 3 + 5? Format the result as follows: >>>{result}<<<'",
        "-t0",
        f"-d{tempdir / 'test_memory.json'}",
    ]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())


@pytest.mark.parametrize("temp", ["-2.5", "2.5"])
def test_invalid_temp_memory(temp: str, tempdir: Path) -> None:
    command = VALGRIND_ROOT + [
        "u",
        "-p'Running a test!'",
        f"-d{tempdir / 'test_memory.json'}",
        f"-t{temp}",
    ]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())


def test_read_from_file(tempdir: Path) -> None:
    prompt = tempdir / "test_read_from_file.txt"
    prompt.write_text("What is 3 + 5? Format the result as follows: >>>{result}<<<")

    command = VALGRIND_ROOT + [
        "-u",
        f"-r{prompt}",
        "-t0",
        f"-d{tempdir / 'test_memory.json'}",
    ]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())


def test_missing_file() -> None:
    command = VALGRIND_ROOT + ["-r/tmp/yU8nnkRs.txt"]

    with pytest.raises(subprocess.CalledProcessError):
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True
        )


@pytest.mark.parametrize("model", ["gpt-3.5-turbo-0301", "gpt-3.5-turbo-0302"])
def test_model(model: str, tempdir: Path) -> None:
    command = VALGRIND_ROOT + [
        "-u",
        "-p'What is 3 + 5?'",
        f"-m{model}",
        f"-d{tempdir / 'test_memory.json'}",
    ]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())
