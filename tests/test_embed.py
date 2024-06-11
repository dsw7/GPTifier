from os import EX_OK
from subprocess import run
from utils import print_stdout, print_stderr


def test_embed_help(command: list[str], capfd) -> None:
    command.extend(["embed", "--help"])
    process = run(command)

    capture = capfd.readouterr()
    print()
    print_stdout(capture.out)
    print_stderr(capture.err)

    assert process.returncode == EX_OK
    assert "SYNOPSIS" in capture.out
