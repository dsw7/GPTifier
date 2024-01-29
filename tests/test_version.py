from datetime import datetime
from json import loads
from os import EX_OK
from subprocess import run


def test_version(command: list[str], capfd) -> None:
    command.extend(["--version"])

    process = run(command)
    assert process.returncode == EX_OK

    cap = capfd.readouterr()
    data = loads(cap.out)

    assert "build_date" in data
    assert "version" in data

    build_date = datetime.strptime(data["build_date"], "%Y-%m-%dT%H:%M:%S")
    now = datetime.now()

    assert build_date.day == now.day
    assert build_date.month == now.month
    assert build_date.year == now.year
