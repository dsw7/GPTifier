from pathlib import Path
from subprocess import run, DEVNULL, PIPE, CalledProcessError
from pytest import mark, fail

VALGRIND_ROOT = ["valgrind", "--error-exitcode=1", "--leak-check=full", "build/gpt"]


def test_basic(tempdir: Path) -> None:
    command = VALGRIND_ROOT + [
        "-u",
        "-p'What is 3 + 5?'",
        "-t0",
        f"-d{tempdir / 'test_memory.json'}",
    ]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())


def test_invalid_temp(tempdir: Path) -> None:
    command = VALGRIND_ROOT + [
        "-u",
        "-p'Running a test!'",
        "-t2.5",
        f"-d{tempdir / 'test_memory.json'}",
    ]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())


def test_read_from_file(tempdir: Path) -> None:
    prompt = tempdir / "test_read_from_file.txt"
    prompt.write_text("What is 3 + 5?")

    command = VALGRIND_ROOT + [
        "-u",
        f"-r{prompt}",
        "-t0",
        f"-d{tempdir / 'test_memory.json'}",
    ]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())


@mark.parametrize("model", ["gpt-3.5-turbo-0301", "gpt-3.5-turbo-0302"])
def test_model(model: str, tempdir: Path) -> None:
    command = VALGRIND_ROOT + [
        "-u",
        "-p'What is 3 + 5?'",
        f"-m{model}",
        f"-d{tempdir / 'test_memory.json'}",
    ]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())
