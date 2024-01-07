from pathlib import Path
from tempfile import TemporaryDirectory
from typing import Generator
from pytest import fixture


def pytest_addoption(parser):
    parser.addoption(
        "--memory", action="store_true", default=False, help="Run Valgrind tests"
    )


@fixture(scope="session")
def command(pytestconfig) -> list[str]:
    if not pytestconfig.getoption("memory"):
        return ["build/gpt", "--no-interactive-export"]

    return [
        "valgrind",
        "--error-exitcode=1",
        "--leak-check=full",
        "build/gpt",
        "--no-interactive-export",
    ]


@fixture
def tempdir() -> Generator[Path, None, None]:
    handle_tempdir = TemporaryDirectory()

    yield Path(handle_tempdir.name)
    handle_tempdir.cleanup()
