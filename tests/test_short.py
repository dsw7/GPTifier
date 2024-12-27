from os import EX_OK
from subprocess import run
from pytest import mark
import utils


@mark.parametrize("option", ["-h", "--help"])
def test_short_help(command: utils.Command, option: str, capfd: utils.Capture) -> None:
    command.extend(["short", option])

    process = run(command)
    stdout, _ = utils.unpack_stdout_stderr(capfd)

    assert process.returncode == EX_OK
    assert "Synopsis" in stdout


@mark.parametrize("option", ["-p", "--prompt="])
def test_short_prompt(
    command: utils.Command, option: str, capfd: utils.Capture
) -> None:
    prompt = '"What is 2 + 2? Format the result as follows: >>>{result}<<<"'
    command.extend(["short", option + prompt])

    process = run(command)
    stdout, _ = utils.unpack_stdout_stderr(capfd)

    assert process.returncode == EX_OK
    assert ">>>4<<<" in stdout


@mark.parametrize("option", ["-r", "--raw"])
def test_short_raw(command: utils.Command, option: str, capfd: utils.Capture) -> None:
    prompt = '"What is 2 + 2?"'
    command.extend(["short", f"--prompt={prompt}", option])

    process = run(command)
    stdout, _ = utils.unpack_stdout_stderr(capfd)

    assert process.returncode == EX_OK
    utils.assert_valid_json(stdout)


def test_missing_prompt(command: utils.Command, capfd: utils.Capture) -> None:
    command.extend(["short"])

    process = run(command)
    _, stderr = utils.unpack_stdout_stderr(capfd)

    assert process.returncode != EX_OK
    assert "Prompt is empty" in stderr
