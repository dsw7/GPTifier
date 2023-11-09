from json import loads
from pathlib import Path
from subprocess import run, CalledProcessError, PIPE, DEVNULL
from tempfile import TemporaryDirectory
from typing import Generator
from pytest import fail, fixture


@fixture
def tempdir() -> Generator[Path, None, None]:
    t_dir = TemporaryDirectory()
    yield Path(t_dir.name)
    t_dir.cleanup()


def test_binary(tempdir: Path) -> None:
    json_file = tempdir / "test.json"

    command = [
        "build/gpt",
        "--prompt='What is 3 + 5? Format the result as follows: >>>{result}<<<'",
        f"--dump={json_file}",
    ]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())

    with json_file.open() as f:
        data = loads(f.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"
