from pathlib import Path
from tempfile import TemporaryDirectory
from typing import Generator
from pytest import fixture


@fixture
def tempdir() -> Generator[Path, None, None]:
    handle_tempdir = TemporaryDirectory()

    yield Path(handle_tempdir.name)
    handle_tempdir.cleanup()
