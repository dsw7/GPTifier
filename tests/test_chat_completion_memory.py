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
    json_file = tempdir / "test_basic_memory.json"

    command = VALGRIND_ROOT + [
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


@pytest.mark.parametrize("temp", ["-2.5", "2.5"])
def test_invalid_temp_memory(temp: str, tempdir: Path) -> None:
    json_file = tempdir / "test_invalid_temp_memory.json"

    command = VALGRIND_ROOT + [
        "u",
        "-p'Running a test!'",
        f"-d{json_file}",
        f"-t{temp}",
    ]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        pytest.fail(exc.stderr.decode())
