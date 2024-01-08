from pathlib import Path
from platform import system
from subprocess import run, DEVNULL, PIPE, CalledProcessError
from pytest import mark, fail, skip

if system() == "Darwin":
    skip("Valgrind not supported on MacOS", allow_module_level=True)

VALGRIND_ROOT = ["valgrind", "--error-exitcode=1", "--leak-check=full", "build/gpt"]
