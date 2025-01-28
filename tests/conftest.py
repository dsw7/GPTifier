from os import environ
from pathlib import Path
from platform import system
from tempfile import TemporaryDirectory, NamedTemporaryFile, gettempdir
from typing import Generator, Any
from utils import EX_MEM_LEAK, Command
from pytest import fixture, exit


def pytest_addoption(parser: Any) -> None:
    parser.addoption(
        "--memory", action="store_true", default=False, help="Run Valgrind tests"
    )


@fixture(scope="session", autouse=True)
def pre_test_checks(pytestconfig: Any) -> None:

    if pytestconfig.getoption("memory"):
        if system() == "Darwin":
            exit("Valgrind not supported on MacOS")

    if "PATH_BIN" not in environ:
        exit("Was the GPTifier binary compiled?")


@fixture(scope="function")
def command(pytestconfig: Any) -> Command:
    path_bin = environ["PATH_BIN"]

    if not pytestconfig.getoption("memory"):
        return Command([path_bin])

    return Command(
        ["valgrind", f"--error-exitcode={EX_MEM_LEAK}", "--leak-check=full", path_bin]
    )


@fixture
def tempdir() -> Generator[Path, None, None]:
    handle_tempdir = TemporaryDirectory()

    yield Path(handle_tempdir.name)
    handle_tempdir.cleanup()


@fixture(scope="function")
def json_file() -> Generator[str, None, None]:
    with NamedTemporaryFile(dir=gettempdir()) as temp_file:
        yield temp_file.name


@fixture(scope="function")
def inputfile() -> Generator[Path, None, None]:
    filename = Path.cwd() / "Inputfile"
    yield filename
    filename.unlink()
