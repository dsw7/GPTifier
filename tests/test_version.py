from datetime import datetime
from json import loads
from os import EX_OK
from subprocess import run
from pytest import mark
from utils import unpack_stdout_stderr, Command, Capture


def run_command(command: Command, option: str, capfd: Capture) -> dict[str, str]:
    command.extend([option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    data: dict[str, str] = loads(stdout)
    return data


@mark.parametrize("option", ["-v", "--version"])
def test_version_version(command: Command, option: str, capfd: Capture) -> None:
    data = run_command(command, option, capfd)
    assert "version" in data


@mark.parametrize("option", ["-v", "--version"])
def test_version_build_date(command: Command, option: str, capfd: Capture) -> None:
    data = run_command(command, option, capfd)
    assert "build_date" in data

    build_date = datetime.strptime(data["build_date"], "%Y-%m-%dT%H:%M:%S")
    now = datetime.now()

    assert build_date.day == now.day
    assert build_date.month == now.month
    assert build_date.year == now.year
