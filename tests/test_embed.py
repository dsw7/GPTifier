from json import loads
from os import EX_OK
from pathlib import Path
from subprocess import run
from pytest import mark
from utils import unpack_stdout_stderr, EX_MEM_LEAK, Command, Capture

RESULTS_JSON = Path.home() / ".gptifier" / "embeddings.gpt"


def load_embedding(json_file: Path) -> tuple[str, str, list[float]]:
    with json_file.open() as f:
        data = loads(f.read())

    return data["model"], data["input"], data["data"][0]["embedding"]


@mark.parametrize("option", ["-h", "--help"])
def test_embed_help(command: Command, option: str, capfd: Capture) -> None:
    command.extend(["embed", option])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "SYNOPSIS" in stdout


def test_basic(command: Command, capfd: Capture) -> None:
    input_text = "What is 3 + 5?"
    model = "text-embedding-ada-002"

    command.extend(["embed", f"-i{input_text}", f"-m{model}"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    model_from_payload, input_from_payload, embedding = load_embedding(RESULTS_JSON)
    assert model_from_payload == model
    assert input_from_payload == input_text
    assert len(embedding) == 1536  # text-embedding-ada-002 dimension


def test_read_from_file(command: Command, capfd: Capture) -> None:
    input_text_file = Path(__file__).resolve().parent / "prompt_basic.txt"
    model = "text-embedding-3-small"

    command.extend(["embed", f"-r{input_text_file}", f"-m{model}"])
    process = run(command)

    unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK

    model_from_payload, input_from_payload, _ = load_embedding(RESULTS_JSON)
    assert model_from_payload == model
    assert input_from_payload == input_text_file.read_text()


def test_read_from_inputfile(command: Command, inputfile: Path, capfd: Capture) -> None:
    inputfile.write_text("A foo that bars!")

    command.extend(["embed"])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert "Found an Inputfile in current working directory!" in stdout


def test_missing_input_file(command: Command, capfd: Capture) -> None:
    command.extend(["embed", "--read-from-file=/tmp/yU8nnkRs.txt"])
    process = run(command)

    _, stderr = unpack_stdout_stderr(capfd)
    assert process.returncode not in (EX_OK, EX_MEM_LEAK)
    assert "Could not open file '/tmp/yU8nnkRs.txt'" in stderr


def test_invalid_model(command: Command, capfd: Capture) -> None:
    command.extend(["embed", "-i'What is 3 + 5?'", "-mfoobar"])
    process = run(command)

    stdout, _ = unpack_stdout_stderr(capfd)
    assert process.returncode == EX_OK
    assert (
        "The model `foobar` does not exist or you do not have access to it." in stdout
    )
