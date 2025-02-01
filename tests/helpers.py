from dataclasses import dataclass
from functools import cache
from json import loads
from os import environ, EX_OK
from subprocess import run, PIPE
from typing import Any

_EX_MEM_LEAK = 2


@cache
def _get_path_to_gptifier_binary() -> list[str]:
    path_bin = environ["PATH_BIN"]

    if "TEST_MEMORY" not in environ:
        return [path_bin]

    return [
        "valgrind",
        f"--error-exitcode={_EX_MEM_LEAK}",
        "--leak-check=full",
        path_bin,
    ]


@dataclass
class Process:
    exit_code: int
    stderr: str
    stdout: str

    def assert_success(self) -> None:
        assert self.exit_code == EX_OK

    def assert_failure(self) -> None:
        assert self.exit_code not in (EX_OK, _EX_MEM_LEAK)

    def load_stdout_to_json(self) -> Any:
        return loads(self.stdout)


def run_process(options: str | list[str]) -> Process:
    command = [*_get_path_to_gptifier_binary()]

    if isinstance(options, str):
        command.append(options)
    elif isinstance(options, list):
        command.extend(options)

    process = run(command, stdout=PIPE, stderr=PIPE)

    return Process(
        exit_code=process.returncode,
        stderr=process.stderr.decode(),
        stdout=process.stdout.decode(),
    )
