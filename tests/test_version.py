from datetime import datetime
from json import loads
from os import EX_OK
from subprocess import run
from pytest import mark
from utils import unpack_stdout_stderr


@mark.parametrize("option", ["-v", "--version"])
def test_version(command: list[str], option: str, capfd) -> None:
    command.extend([option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    data = loads(stdout)
    assert "build_date" in data
    assert "version" in data

    build_date = datetime.strptime(data["build_date"], "%Y-%m-%dT%H:%M:%S")
    now = datetime.now()

    assert build_date.day == now.day
    assert build_date.month == now.month
    assert build_date.year == now.year
