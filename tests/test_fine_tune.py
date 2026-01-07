from pathlib import Path
import pytest
import utils


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help(option: str) -> None:
    stdout = utils.assert_command_success("fine-tune", option)
    assert "Manage all fine-tuning operations" in stdout


def test_no_args_or_opts_upload_file() -> None:
    stderr = utils.assert_command_failure("fine-tune", "upload-file")
    assert "A fine tuning file needs to be provided" in stderr


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help_fine_tune_upload_file(option: str) -> None:
    stdout = utils.assert_command_success("fine-tune", "upload-file", option)
    assert "Upload a fine-tuning file" in stdout


def test_upload_missing_file() -> None:
    stderr = utils.assert_command_failure("fine-tune", "upload-file", "foobar")
    assert "Failed to open/read local data from file/application" in stderr


def test_upload_empty_filename() -> None:
    stderr = utils.assert_command_failure("fine-tune", "upload-file", "")
    assert "Filename is empty" in stderr


def test_upload_invalid_file() -> None:
    # try to upload a python file
    input_text_file = Path(__file__).resolve().parent / "utils.py"
    stderr = utils.assert_command_failure(
        "fine-tune", "upload-file", str(input_text_file)
    )
    assert "Invalid file format for Fine-Tuning API. Must be .jsonl" in stderr


def test_no_args_or_opts_create_job() -> None:
    stderr = utils.assert_command_failure("fine-tune", "create-job")
    assert "A fine tuning file ID and model needs to be provided" in stderr


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help_fine_tune_create_job(option: str) -> None:
    stdout = utils.assert_command_success("fine-tune", "create-job", option)
    assert "Create a fine-tuning job" in stdout


def test_create_job_invalid_option() -> None:
    stderr = utils.assert_command_failure("fine-tune", "create-job", "foobar")
    assert "Unknown option: 'foobar'" in stderr


def test_create_job_invalid_training_file() -> None:
    stderr = utils.assert_command_failure(
        "fine-tune", "create-job", "foobar", "gpt-3.5-turbo"
    )
    assert "invalid training_file: foobar" in stderr


def test_create_job_empty_training_file() -> None:
    stderr = utils.assert_command_failure(
        "fine-tune", "create-job", "", "gpt-3.5-turbo"
    )
    assert "Training file ID argument is empty" in stderr


def test_create_job_empty_model() -> None:
    stderr = utils.assert_command_failure("fine-tune", "create-job", "foobar", "")
    assert "Model argument is empty" in stderr


def test_no_args_or_opts_delete_model() -> None:
    stderr = utils.assert_command_failure("fine-tune", "delete-model")
    assert "A model ID needs to be provided" in stderr


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help_fine_tune_delete_model(option: str) -> None:
    stdout = utils.assert_command_success("fine-tune", "delete-model", option)
    assert "Delete a fine-tuned model" in stdout


def test_delete_model_missing_model() -> None:
    stderr = utils.assert_command_failure("fine-tune", "delete-model", "foobar")
    assert "The model 'foobar' does not exist" in stderr


def test_delete_model_empty_model_id_arg() -> None:
    stderr = utils.assert_command_failure("fine-tune", "delete-model", "")
    assert "Model ID argument is empty" in stderr


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help_fine_tune_list_jobs(option: str) -> None:
    stdout = utils.assert_command_success("fine-tune", "list-jobs", option)
    assert "List fine-tuning jobs" in stdout


@pytest.mark.parametrize("option", ["-j", "--json"])
def test_list_jobs_raw_json(option: str) -> None:
    stdout = utils.assert_command_success("fine-tune", "list-jobs", option)
    utils.load_stdout_to_json(stdout)


@pytest.mark.parametrize("option", ["-l1", "--limit=1"])
def test_list_jobs(option: str) -> None:
    stdout = utils.assert_command_success("fine-tune", "list-jobs", option)
    assert "No limit passed with --limit flag." not in stdout


def test_list_jobs_stoi() -> None:
    stderr = utils.assert_command_failure("fine-tune", "list-jobs", "--limit=abc")
    assert "Failed to convert 'abc' to int" in stderr


def test_list_jobs_empty_limit() -> None:
    stderr = utils.assert_command_failure("fine-tune", "list-jobs", "--limit=")
    assert "Limit is empty" in stderr
