from datetime import datetime
from os import getenv
from unittest import TestCase
import pytest
import utils


def test_root_help_long() -> None:
    utils.assert_command_success("--help")


def test_root_help_short() -> None:
    utils.assert_command_success("-h")


def test_root_help_empty_argv() -> None:
    # i.e. running just $ gpt
    stderr = utils.assert_command_failure()
    assert "A command line OpenAI toolkit." in stderr


def test_copyright() -> None:
    stdout = utils.assert_command_success("--help")
    assert f"-- Copyright (C) 2023-{datetime.now().year} by David Weber" in stdout


def test_unknown_command() -> None:
    stderr = utils.assert_command_failure("foobar")
    assert stderr == "Received unknown command. Re-run with -h or --help"


def _is_memory_test() -> bool:
    if getenv("TEST_MEMORY") is not None:
        return True

    return False


@pytest.mark.skipif(
    not _is_memory_test(),
    reason="Test will not raise AssertionError if running outside of Valgrind context",
)
def test_catch_memory_leak() -> None:
    # Test that our custom assertSuccess catches a fake memory leak
    with pytest.raises(AssertionError):
        stdout = utils.assert_command_success("test", "leak")
        assert stdout == "5"


def test_reuse_curl_handle() -> None:
    # Test that curl handle can be reused in GPTifier's network layer
    stdout = utils.assert_command_success("test", "ccc")
    TestCase().assertDictEqual(
        utils.load_stdout_to_json(stdout),
        {"result_1": ">>>10<<<", "result_2": ">>>10<<<", "result_3": ">>>10<<<"},
    )
