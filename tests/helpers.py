from dataclasses import dataclass
from functools import cache
from os import environ, EX_OK
from subprocess import run, PIPE


@cache
def _get_path_to_gptifier_binary() -> str:
    return environ["PATH_BIN"]


@dataclass
class Process:
    exit_code: int
    stderr: str
    stdout: str

    def assert_success(self) -> None:
        assert self.exit_code == EX_OK

    def assert_failure(self) -> None:
        assert self.exit_code != EX_OK


def run_process(options: str) -> Process:
    command = [_get_path_to_gptifier_binary(), options]
    process = run(command, stdout=PIPE, stderr=PIPE)

    return Process(
        exit_code=process.returncode,
        stderr=process.stderr.decode(),
        stdout=process.stdout.decode(),
    )
