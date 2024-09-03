from os import EX_OK
from subprocess import run
from pytest import mark
from utils import unpack_stdout_stderr, Command


@mark.parametrize("option", ["-h", "--help"])
def test_models_help(command: Command, option: str, capfd) -> None:
    command.extend(["models", option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "SYNOPSIS" in stdout


def test_models(command: Command, capfd) -> None:
    command.extend(["models"])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    models = stdout.split("\n")
    assert len(models) > 1
