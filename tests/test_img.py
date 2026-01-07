import pytest
import utils


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help(option: str) -> None:
    stdout = utils.assert_command_success("img", option)
    assert "Generate an image from a prompt" in stdout


def test_missing_prompt_file() -> None:
    stderr = utils.assert_command_failure("img")
    assert "No prompt file provided. Cannot proceed" in stderr


def test_empty_prompt_file() -> None:
    stderr = utils.assert_command_failure("img", "")
    assert "Could not read from file. Filename is empty" in stderr
