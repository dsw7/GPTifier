import re
import pytest
import utils

SEARCH_PATTERN = r"Creation time\s+Owner\s+Model ID"


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help(option: str) -> None:
    stdout = utils.assert_command_success("models", option)
    assert "List available OpenAI or user models" in stdout


def test_models_openai() -> None:
    stdout = utils.assert_command_success("models")
    assert re.search(stdout, SEARCH_PATTERN) is not None


@pytest.mark.parametrize("option", ["-u", "--user"])
def test_models_user(option: str) -> None:
    stdout = utils.assert_command_success("models", option)
    assert re.search(stdout, SEARCH_PATTERN) is not None


@pytest.mark.parametrize("option", ["-j", "--json"])
def test_models_raw_json(option: str) -> None:
    stdout = utils.assert_command_success("models", option)
    utils.load_stdout_to_json(stdout)
