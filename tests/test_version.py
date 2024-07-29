from datetime import datetime
from json import loads
from os import EX_OK
from subprocess import run
from pytest import mark
from utils import print_stdout, print_stderr


@mark.parametrize("option", ["-v", "--version"])
def test_version(command: list[str], capfd) -> None:
    command.extend([option])
    process = run(command)

    capture = capfd.readouterr()
    print()
    print_stdout(capture.out)
    print_stderr(capture.err)

    assert process.returncode == EX_OK

    data = loads(capture.out)
    assert "build_date" in data
    assert "version" in data

    build_date = datetime.strptime(data["build_date"], "%Y-%m-%dT%H:%M:%S")
    now = datetime.now()

    assert build_date.day == now.day
    assert build_date.month == now.month
    assert build_date.year == now.year
