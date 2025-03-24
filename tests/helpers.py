from dataclasses import dataclass
from functools import cache
from json import loads
from os import getenv, EX_OK
from subprocess import run, PIPE
from typing import Any

_EX_MEM_LEAK = 2


@cache
def get_path_to_gptifier_binary() -> str:
    path_bin: str | None = getenv("PATH_BIN")

    if path_bin is None:
        raise SystemExit("Could not locate PATH_BIN environment variable")

    return path_bin


@cache
def is_memory_test() -> bool:
    if getenv("TEST_MEMORY") is not None:
        return True

    return False


@cache
def _get_test_command() -> list[str]:
    if is_memory_test():
        return [
            "valgrind",
            f"--error-exitcode={_EX_MEM_LEAK}",
            "--leak-check=full",
            get_path_to_gptifier_binary(),
        ]

    return [get_path_to_gptifier_binary()]


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
    command = []
    command.extend(_get_test_command())

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
