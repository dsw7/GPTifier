from os import EX_OK
from subprocess import run


def test_models_help(command: list[str], capfd) -> None:
    command.extend(["models", "--help"])

    process = run(command)
    assert process.returncode == EX_OK

    cap = capfd.readouterr()
    assert "SYNOPSIS" in cap.out


def test_models(command: list[str], capfd) -> None:
    command.extend(["models"])

    process = run(command)
    assert process.returncode == EX_OK

    cap = capfd.readouterr()
    models = cap.out.split("\n")

    assert len(models) > 1
    assert '> "gpt-4"' in models
