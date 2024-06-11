from json import loads
from os import EX_OK
from pathlib import Path
from subprocess import run
from utils import print_stdout, print_stderr, print_stdout_stderr

RESULTS_JSON = Path.home() / ".gptifier" / "embeddings.gpt"


def test_basic(command: list[str], capfd) -> None:
    ada_002_vector_dimension = 1536
    input_text = "What is 3 + 5?"
    model = "text-embedding-ada-002"

    command.extend(["embed", f"-i{input_text}", f"-m{model}"])
    process = run(command)

    print_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    with open(RESULTS_JSON) as f_json:
        data = loads(f_json.read())
        assert data["model"] == model
        assert data["input"] == input_text
        assert len(data["data"][0]["embedding"]) == ada_002_vector_dimension


def test_read_from_file(command: list[str], capfd) -> None:
    input_text_file = Path(__file__).resolve().parent / "prompt_basic.txt"
    model = "text-embedding-3-small"

    command.extend(["embed", f"-r{input_text_file}", f"-m{model}"])
    process = run(command)

    print_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    with open(RESULTS_JSON) as f_json:
        data = loads(f_json.read())
        assert data["model"] == model
        assert data["input"] == input_text_file.read_text()


def test_invalid_model(command: list[str], capfd) -> None:
    command.extend(["embed", "-i'What is 3 + 5?'", "-mfoobar"])
    process = run(command)

    print_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    with open(RESULTS_JSON) as f_json:
        data = loads(f_json.read())
        assert (
            data["error"]["message"]
            == "The model `foobar` does not exist or you do not have access to it."
        )


def test_embed_help(command: list[str], capfd) -> None:
    command.extend(["embed", "--help"])
    process = run(command)

    capture = capfd.readouterr()
    print()
    print_stdout(capture.out)
    print_stderr(capture.err)

    assert process.returncode == EX_OK
    assert "SYNOPSIS" in capture.out
