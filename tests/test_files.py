from re import search
from pathlib import Path
import pytest
import utils


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help_files(option: str) -> None:
    stdout = utils.assert_command_success("files", option)
    assert "Manage files uploaded to OpenAI" in stdout


def test_upload_then_delete() -> None:
    jsonl_file = Path(__file__).resolve().parent / "test_files" / "dummy.jsonl"

    stdout = utils.assert_command_success("fine-tune", "upload-file", str(jsonl_file))
    last_line = stdout.strip().rsplit("\n", maxsplit=1)[-1]
    file_id = last_line.split(": ")[-1]

    utils.assert_command_success("files", "delete", file_id)


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help_files_list(option: str) -> None:
    stdout = utils.assert_command_success("files", "list", option)
    assert "Get list of files uploaded to OpenAI servers" in stdout


PATTERN = r"File ID\s+Filename\s+Creation time\s+Purpose"


def test_files_list_default() -> None:
    stdout = utils.assert_command_success("files")
    assert search(PATTERN, stdout) is not None


def test_files_list() -> None:
    stdout = utils.assert_command_success("files", "list")
    assert search(PATTERN, stdout) is not None


@pytest.mark.parametrize("option", ["-j", "--json"])
def test_files_list_raw_json(option: str) -> None:
    stdout = utils.assert_command_success("files", "list", option)
    utils.load_stdout_to_json(stdout)


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help_files_delete(option: str) -> None:
    stdout = utils.assert_command_success("files", "delete", option)
    assert "Delete one or more uploaded files" in stdout


def test_files_delete() -> None:
    stderr = utils.assert_command_failure("files", "delete", "foobar")
    assert (
        'Failed to delete file with ID: foobar. The error was: "No such File object: foobar"\n'
        "One or more failures occurred when deleting files\n"
    ) in stderr


def test_files_delete_no_ids() -> None:
    stderr = utils.assert_command_failure("files", "delete")
    assert "One or more file IDs need to be provided" in stderr


def test_files_delete_one_empty_id() -> None:
    stderr = utils.assert_command_failure("files", "delete", "")
    assert "Cannot delete file. ID is empty" in stderr


def test_files_delete_multiple_empty_ids() -> None:
    stderr = utils.assert_command_failure("files", "delete", "", "", "")
    assert 3 * "Cannot delete file. ID is empty\n" in stderr


def test_files_delete_multiple() -> None:
    stderr = utils.assert_command_failure("files", "delete", "spam", "ham", "eggs")
    assert (
        'Failed to delete file with ID: spam. The error was: "No such File object: spam"\n'
        'Failed to delete file with ID: ham. The error was: "No such File object: ham"\n'
        'Failed to delete file with ID: eggs. The error was: "No such File object: eggs"\n'
        "One or more failures occurred when deleting files\n"
    ) in stderr
