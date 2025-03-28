from dataclasses import dataclass
from functools import cache
from json import loads
from os import getenv, EX_OK
from subprocess import run, PIPE
from typing import Any
from unittest import TestCase


@cache
def get_path_to_gptifier_binary() -> str:
    path_bin: str | None = getenv("PATH_BIN")

    if path_bin is None:
        raise SystemExit("Could not locate PATH_BIN environment variable")

    return path_bin


@cache
def is_memory_test() -> bool:
    return True


@dataclass
class Process:
    exit_code: int
    stderr: str
    stdout: str

    def assert_success(self) -> None:
        assert self.exit_code == EX_OK

    def assert_failure(self) -> None:
        assert self.exit_code != EX_OK

    def load_stdout_to_json(self) -> Any:
        return loads(self.stdout)


class TestCaseExtended(TestCase):
    def _assertSuccess(self, *args: str) -> None:
        command = [get_path_to_gptifier_binary()]
        command.extend(args)

        process = run(command, stdout=PIPE, stderr=PIPE)
        assert process.returncode == EX_OK

    def _assertNoMemoryLeak(self, *args: str) -> None:
        ex_mem_leak = 2
        command = [
            "valgrind",
            f"--error-exitcode={ex_mem_leak}",
            "--leak-check=full",
            get_path_to_gptifier_binary(),
        ]
        command.extend(args)

        process = run(command, stdout=PIPE, stderr=PIPE)
        assert process.returncode != ex_mem_leak

    def assertSuccess(self, *args: str) -> None:

        if is_memory_test():
            self._assertNoMemoryLeak(*args)
        else:
            self._assertSuccess(*args)
