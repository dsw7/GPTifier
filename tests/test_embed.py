from json import loads
from os import EX_OK
from pathlib import Path
from subprocess import run
from utils import print_stdout, print_stderr, print_stdout_stderr


def test_basic(command: list[str], capfd) -> None:
    model = "text-embedding-ada-002"
    ada_002_vector_dimension = 1536

    command.extend(["embed", "-i'What is 3 + 5?'", f"-m{model}"])
    process = run(command)

    print_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    results_json = Path.home() / ".gptifier" / "embeddings.gpt"

    with open(results_json) as f_json:
        data = loads(f_json.read())
        assert data["model"] == model
        assert len(data["data"][0]["embedding"]) == ada_002_vector_dimension


def test_embed_help(command: list[str], capfd) -> None:
    command.extend(["embed", "--help"])
    process = run(command)

    capture = capfd.readouterr()
    print()
    print_stdout(capture.out)
    print_stderr(capture.err)

    assert process.returncode == EX_OK
    assert "SYNOPSIS" in capture.out
