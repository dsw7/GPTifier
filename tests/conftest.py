from pathlib import Path
from platform import system
from tempfile import TemporaryDirectory, NamedTemporaryFile, gettempdir
from typing import Generator
from pytest import fixture, exit
from utils import EX_MEM_LEAK


def pytest_addoption(parser):
    parser.addoption(
        "--memory", action="store_true", default=False, help="Run Valgrind tests"
    )


@fixture(scope="session", autouse=True)
def pre_test_checks(pytestconfig) -> None:
    if pytestconfig.getoption("memory"):
        if system() == "Darwin":
            exit("Valgrind not supported on MacOS")

    if not Path("build/gpt").exists():
        exit("Was the GPTifier binary compiled?")


@fixture(scope="function")
def command(pytestconfig) -> list[str]:
    if not pytestconfig.getoption("memory"):
        return ["build/gpt"]

    return [
        "valgrind",
        f"--error-exitcode={EX_MEM_LEAK}",
        "--leak-check=full",
        "build/gpt",
    ]


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
