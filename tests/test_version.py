from datetime import datetime
import pytest
import utils


def test_correct_version() -> None:
    # I.e. ensure CMakeLists.txt project VERSION is updated
    stdout = utils.assert_command_success("-v")
    json = utils.load_stdout_to_json(stdout)
    assert "version" in json
    assert json["version"] == "1.7.0"


@pytest.mark.parametrize("option", ["-v", "--version"])
def test_version_version(option: str) -> None:
    stdout = utils.assert_command_success(option)
    json = utils.load_stdout_to_json(stdout)
    assert "version" in json


@pytest.mark.parametrize("option", ["-v", "--version"])
def test_version_build_type(option: str) -> None:
    stdout = utils.assert_command_success(option)
    json = utils.load_stdout_to_json(stdout)
    assert "build_type" in json
    assert json["build_type"] == "Testing"


@pytest.mark.parametrize("option", ["-v", "--version"])
def test_version_build_date(option: str) -> None:
    stdout = utils.assert_command_success(option)
    json = utils.load_stdout_to_json(stdout)
    assert "build_date" in json

    build_date = datetime.strptime(json["build_date"], "%Y-%m-%dT%H:%M:%S")
    now = datetime.now()

    assert build_date.day == now.day
    assert build_date.month == now.month
    assert build_date.year == now.year
