from os import environ
import pytest
import utils

pytestmark = pytest.mark.skipif(
    "OPENAI_ADMIN_KEY" not in environ,
    reason="OPENAI_ADMIN_KEY does not exist in environment",
)


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help(option: str) -> None:
    stdout = utils.assert_command_success("costs", option)
    assert "Get OpenAI usage details" in stdout


def test_costs() -> None:
    stdout = utils.assert_command_success("costs", "--days=4")
    assert "Overall usage" in stdout


def test_costs_count() -> None:
    stdout = utils.assert_command_success("costs", "--json", "--days=3")
    json = utils.load_stdout_to_json(stdout)
    assert len(json["data"]) == 3


@pytest.mark.parametrize("option", ["-j", "--json"])
def test_costs_raw_json(option: str) -> None:
    stdout = utils.assert_command_success("costs", option, "--days=3")
    utils.load_stdout_to_json(stdout)


def test_costs_invalid_days_empty() -> None:
    stderr = utils.assert_command_failure("costs", "--days=")
    assert "Days argument is empty" in stderr


def test_costs_invalid_days_stoi() -> None:
    stderr = utils.assert_command_failure("costs", "--days=abc")
    assert "Failed to convert 'abc' to int" in stderr
