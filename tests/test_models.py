from os import EX_OK
from subprocess import run
from pytest import mark
import utils


@mark.parametrize("option", ["-h", "--help"])
def test_models_help(command: utils.Command, option: str, capfd: utils.Capture) -> None:
    command.extend(["models", option])
    process = run(command)

    stdout, _ = utils.unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "Synopsis" in stdout


def test_models(command: utils.Command, capfd: utils.Capture) -> None:
    command.extend(["models"])
    process = run(command)

    stdout, _ = utils.unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    models = stdout.split("\n")
    assert len(models) > 1


@mark.parametrize("option", ["-r", "--raw"])
def test_models_raw(command: utils.Command, option: str, capfd: utils.Capture) -> None:
    command.extend(["models", option])
    process = run(command)

    stdout, _ = utils.unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    utils.assert_valid_json(stdout)
