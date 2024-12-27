from json import loads, JSONDecodeError
from typing import NewType, Any, TypeAlias
from colorama import Fore, Style
from pytest import CaptureFixture, fail

Capture: TypeAlias = CaptureFixture[Any]
Command = NewType("Command", list[str])
PrefixStderr = Fore.RED + "stderr >>> " + Style.RESET_ALL
PrefixStdout = Fore.GREEN + "stdout >>> " + Style.RESET_ALL

# Custom Valgrind exit code such that we can delimit stderr being tested from Valgrind stderr
EX_MEM_LEAK = 3


def _print_stdout(raw_stdout: str) -> None:
    if not raw_stdout:
        return

    for line in raw_stdout.split("\n"):
        print(PrefixStdout + line)


def _print_stderr(raw_stderr: str) -> None:
    if not raw_stderr:
        return

    for line in raw_stderr.split("\n"):
        print(PrefixStderr + line)


def unpack_stdout_stderr(capture: Capture) -> tuple[str, str]:
    output = capture.readouterr()
    print()

    _print_stdout(output.out)
    _print_stderr(output.err)
    return output.out, output.err


def assert_valid_json(stdout: str) -> None:
    try:
        loads(stdout)
    except JSONDecodeError:
        fail("Test should not have failed on decoding JSON")
