from colorama import Fore, Style

PREFIX_STDOUT = Fore.GREEN + "stdout >>> " + Style.RESET_ALL
PREFIX_STDERR = Fore.RED + "stderr >>> " + Style.RESET_ALL


def print_stdout(raw_stdout: str) -> None:
    if not raw_stdout:
        return

    for line in raw_stdout.split("\n"):
        print(PREFIX_STDOUT + line)


def print_stderr(raw_stderr: str) -> None:
    if not raw_stderr:
        return

    for line in raw_stderr.split("\n"):
        print(PREFIX_STDERR + line)

def print_stdout_stderr(capture) -> None:
    output = capture.readouterr()
    print()

    print_stdout(output.out)
    print_stderr(output.err)
