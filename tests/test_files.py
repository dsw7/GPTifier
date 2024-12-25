from os import EX_OK
from subprocess import run
from pytest import mark
from utils import unpack_stdout_stderr, Command, Capture


@mark.parametrize("option", ["-h", "--help"])
def test_files_help(command: Command, option: str, capfd: Capture) -> None:
    command.extend(["files", option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "Synopsis" in stdout


@mark.parametrize("option", ["-h", "--help"])
@mark.parametrize("subcommand", ["list", "delete"])
def test_files_subcommand_help(
    command: Command, subcommand: str, option: str, capfd: Capture
) -> None:
    command.extend(["files", subcommand, option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "Synopsis" in stdout


def test_files_list(command: Command, capfd: Capture) -> None:
    command.extend(["files", "list"])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "File ID" in stdout


def test_files_delete(command: Command, capfd: Capture) -> None:
    command.extend(["files", "delete", "foobar"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert (
        'Failed to delete file with ID: foobar. The error was: "No such File object: foobar"\n'
        "One or more failures occurred when deleting files\n"
    ) in stderr


def test_files_delete_multiple(command: Command, capfd: Capture) -> None:
    command.extend(["files", "delete", "spam", "ham", "eggs"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert (
        'Failed to delete file with ID: spam. The error was: "No such File object: spam"\n'
        'Failed to delete file with ID: ham. The error was: "No such File object: ham"\n'
        'Failed to delete file with ID: eggs. The error was: "No such File object: eggs"\n'
        "One or more failures occurred when deleting files\n"
    ) in stderr
