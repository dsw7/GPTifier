from os import EX_OK
from subprocess import run
from pytest import mark
from utils import unpack_stdout_stderr, Command, Capture


@mark.parametrize("option", ["-h", "--help"])
def test_fine_tune_help(command: Command, option: str, capfd: Capture) -> None:
    command.extend(["fine-tune", option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "Synopsis" in stdout


@mark.parametrize("option", ["-h", "--help"])
@mark.parametrize("subcommand", ["upload-file", "create-job", "delete-model"])
def test_fine_tune_subcommand_help(
    command: Command, subcommand: str, option: str, capfd: Capture
) -> None:
    command.extend(["fine-tune", subcommand, option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "Synopsis" in stdout


def test_fine_tune_upload_missing_file(command: Command, capfd: Capture) -> None:
    command.extend(["fine-tune", "upload-file", "foobar"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert "Failed to open/read local data from file/application" in stderr


def test_fine_tune_upload_invalid_file(command: Command, capfd: Capture) -> None:
    command.extend(["fine-tune", "upload-file", "tests/prompt_basic.txt"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert "Invalid file format for Fine-Tuning API. Must be .jsonl" in stderr


def test_fine_tune_create_job_invalid_params(command: Command, capfd: Capture) -> None:
    command.extend(["fine-tune", "create-job", "--model=foobar", "--file-id=foobar"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert "invalid training_file: foobar" in stderr


def test_fine_tune_delete_model_missing_model(command: Command, capfd: Capture) -> None:
    command.extend(["fine-tune", "delete-model", "foobar"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert "The model 'foobar' does not exist" in stderr
