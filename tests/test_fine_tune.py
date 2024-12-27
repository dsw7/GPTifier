from os import EX_OK
from subprocess import run
from pytest import mark
import utils


@mark.parametrize("option", ["-h", "--help"])
def test_fine_tune_help(
    command: utils.Command, option: str, capfd: utils.Capture
) -> None:
    command.extend(["fine-tune", option])
    process = run(command)

    stdout, _ = utils.unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "Synopsis" in stdout


@mark.parametrize("option", ["-h", "--help"])
@mark.parametrize(
    "subcommand", ["upload-file", "create-job", "delete-model", "list-jobs"]
)
def test_fine_tune_subcommand_help(
    command: utils.Command, subcommand: str, option: str, capfd: utils.Capture
) -> None:
    command.extend(["fine-tune", subcommand, option])
    process = run(command)

    stdout, _ = utils.unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "Synopsis" in stdout


def test_fine_tune_upload_missing_file(
    command: utils.Command, capfd: utils.Capture
) -> None:
    command.extend(["fine-tune", "upload-file", "foobar"])
    process = run(command)

    _, stderr = utils.unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert "Failed to open/read local data from file/application" in stderr


def test_fine_tune_upload_invalid_file(
    command: utils.Command, capfd: utils.Capture
) -> None:
    command.extend(["fine-tune", "upload-file", "tests/prompt_basic.txt"])
    process = run(command)

    _, stderr = utils.unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert "Invalid file format for Fine-Tuning API. Must be .jsonl" in stderr


def test_fine_tune_create_job_invalid_params(
    command: utils.Command, capfd: utils.Capture
) -> None:
    command.extend(["fine-tune", "create-job", "--model=foobar", "--file-id=foobar"])
    process = run(command)

    _, stderr = utils.unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert "invalid training_file: foobar" in stderr


def test_fine_tune_delete_model_missing_model(
    command: utils.Command, capfd: utils.Capture
) -> None:
    command.extend(["fine-tune", "delete-model", "foobar"])
    process = run(command)

    _, stderr = utils.unpack_stdout_stderr(capfd)
    assert process.returncode != EX_OK
    assert "The model 'foobar' does not exist" in stderr


@mark.parametrize("option", ["-r", "--raw"])
def test_fine_tune_list_jobs_raw(
    command: utils.Command, option: str, capfd: utils.Capture
) -> None:
    command.extend(["fine-tune", "list-jobs", option])
    process = run(command)

    stdout, _ = utils.unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    utils.assert_valid_json(stdout)


@mark.parametrize("option", ["-l1", "--limit=1"])
def test_fine_tune_list_jobs(
    command: utils.Command, option: str, capfd: utils.Capture
) -> None:
    command.extend(["fine-tune", "list-jobs", option])
    process = run(command)

    stdout, _ = utils.unpack_stdout_stderr(capfd)
    assert "No limit passed with --limit flag." not in stdout
    assert process.returncode == EX_OK
