from os import EX_OK
from subprocess import run
from pytest import mark
from utils import print_stdout, print_stderr


@mark.parametrize("option", ["-h", "--help"])
def test_models_help(command: list[str], option: str, capfd) -> None:
    command.extend(["models", option])
    process = run(command)

    capture = capfd.readouterr()
    print()
    print_stdout(capture.out)
    print_stderr(capture.err)

    assert process.returncode == EX_OK
    assert "SYNOPSIS" in capture.out


def test_models(command: list[str], capfd) -> None:
    command.extend(["models"])
    process = run(command)

    capture = capfd.readouterr()
    print()
    print_stdout(capture.out)
    print_stderr(capture.err)

    assert process.returncode == EX_OK
    models = capture.out.split("\n")
    assert len(models) > 1
