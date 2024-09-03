from json import loads
from typing import NewType
from colorama import Fore, Style

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


def unpack_stdout_stderr(capture) -> tuple[str, str]:
    output = capture.readouterr()
    print()

    _print_stdout(output.out)
    _print_stderr(output.err)
    return output.out, output.err


def load_error(json_file: str) -> str:
    with open(json_file) as f:
        contents = loads(f.read())

    return contents["error"]["message"]
