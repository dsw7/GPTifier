from dataclasses import dataclass
from functools import cache
from json import loads
from os import getenv, EX_OK
from subprocess import run, PIPE, CompletedProcess
from typing import Any
from unittest import TestCase


@cache
def _get_path_to_gptifier_binary() -> str:
    path_bin: str | None = getenv("PATH_BIN")

    if path_bin is None:
        raise SystemExit("Could not locate PATH_BIN environment variable")

    return path_bin


@cache
def is_memory_test() -> bool:
    if getenv("TEST_MEMORY") is not None:
        return True

    return False


@dataclass
class _Process:
    stderr: str
    stdout: str

    def load_stdout_to_json(self) -> Any:
        return loads(self.stdout)


def _get_debug_messages(process: CompletedProcess[bytes]) -> str:
    message = f"Program exited with code {process.returncode}.\n"

    message += "The failing command is:\n"
    message += f"```\n{' '.join(process.args)}\n```\n"

    for line in process.stdout.decode().splitlines():
        message += f"stdout > {line}\n"

    message += "\n"

    for line in process.stderr.decode().splitlines():
        message += f"stderr > {line}\n"

    return message


class TestCaseExtended(TestCase):

    def _assertSuccess(self, *args: str) -> _Process:
        command = [_get_path_to_gptifier_binary()]
        command.extend(args)

        process = run(command, stdout=PIPE, stderr=PIPE)
        assert process.returncode == EX_OK, _get_debug_messages(process)

        return _Process(
            stderr=process.stderr.decode(),
            stdout=process.stdout.decode(),
        )

    def _assertNoMemoryLeak(self, *args: str) -> _Process:
        ex_mem_leak = 2
        command = [
            "valgrind",
            f"--error-exitcode={ex_mem_leak}",
            "--leak-check=full",
            _get_path_to_gptifier_binary(),
        ]
        command.extend(args)

        process = run(command, stdout=PIPE, stderr=PIPE)
        assert (
            process.returncode != ex_mem_leak
        ), f"Program is leaking memory.\nRun ```{' '.join(process.args)}``` to inspect leak"

        return _Process(
            stderr=process.stderr.decode(),
            stdout=process.stdout.decode(),
        )

    def assertSuccess(self, *args: str) -> _Process:
        if is_memory_test():
            return self._assertNoMemoryLeak(*args)

        return self._assertSuccess(*args)

    def assertFailure(self, *args: str) -> _Process:
        command = [_get_path_to_gptifier_binary()]
        command.extend(args)

        process = run(command, stdout=PIPE, stderr=PIPE)
        assert process.returncode == 1

        return _Process(
            stderr=process.stderr.decode(),
            stdout=process.stdout.decode(),
        )
