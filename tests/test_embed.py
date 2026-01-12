from dataclasses import dataclass
from json import loads
from pathlib import Path
from tempfile import gettempdir
from typing import Generator
import pytest
import utils


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help(option: str) -> None:
    stdout = utils.assert_command_success("embed", option)
    assert "Get embedding representing a block of text" in stdout


def test_missing_input_file() -> None:
    stderr = utils.assert_command_failure("embed", "--read-from-file=/tmp/yU8nnkRs.txt")
    assert "Unable to open '/tmp/yU8nnkRs.txt'" in stderr


def test_empty_input_file() -> None:
    stderr = utils.assert_command_failure(
        "embed", "--read-from-file=", "--output-file=/tmp/results.txt"
    )
    assert "Could not read from file. Filename is empty" in stderr


def test_empty_input() -> None:
    stderr = utils.assert_command_failure("embed", "--input=")
    assert "No input text provided anywhere" in stderr


def test_empty_model() -> None:
    stderr = utils.assert_command_failure("embed", "--input=foobar", "--model=")
    assert "Model is empty" in stderr


def test_empty_output_file() -> None:
    stderr = utils.assert_command_failure("embed", "--input=foobar", "--output-file=")
    assert "Output file argument provided with no value" in stderr


@pytest.mark.test_openai
def test_non_existent_model_openai() -> None:
    stderr = utils.assert_command_failure("embed", "-i'What is 3 + 5?'", "-mfoobar")
    assert (
        "The model `foobar` does not exist or you do not have access to it." in stderr
    )


@pytest.mark.test_ollama
def test_non_existent_model_ollama() -> None:
    stderr = utils.assert_command_failure(
        "embed", "-i'What is 3 + 5?'", "-mfoobar", "--use-local"
    )
    assert 'model "foobar" not found, try pulling it first' in stderr


@pytest.mark.test_openai
@pytest.mark.parametrize(
    "model",
    [
        "dall-e-3",
        "gpt-3.5-turbo",
        "gpt-4",
        "gpt-4.1",
        "gpt-4o",
        "o1",
        "o3",
        "tts-1",
        "whisper-1",
    ],
)
def test_wrong_models_openai(model: str) -> None:
    stderr = utils.assert_command_failure(
        "embed", "--input='A foo that bars'", f"-m{model}"
    )
    assert "You are not allowed to generate embeddings from this model" in stderr


@pytest.mark.test_ollama
def test_wrong_models_ollama() -> None:
    stderr = utils.assert_command_failure(
        "embed", "--input='A foo that bars'", "--model=gemma3:latest", "--use-local"
    )
    assert "this model does not support embeddings" in stderr


@dataclass
class _Embedding:
    embedding: list[float]
    model: str
    source: str
    text: str


def _load_embedding(results_file: Path) -> _Embedding:
    with results_file.open() as f:
        data = loads(f.read())

    return _Embedding(
        model=data["model"],
        text=data["input"],
        embedding=data["embedding"],
        source=data["source"],
    )


@pytest.fixture
def embed_test_files() -> Generator[tuple[Path, Path], None, None]:
    input_file = Path(__file__).resolve().parent / "test_embed" / "lorem.txt"
    output_file = Path(gettempdir()) / "result.gpt"
    yield input_file, output_file

    if output_file.exists():
        output_file.unlink()


@pytest.mark.test_openai
def test_get_embedding_openai(embed_test_files: tuple[Path, Path]) -> None:
    input_file, output_file = embed_test_files
    model = "text-embedding-3-small"
    utils.assert_command_success(
        "embed", f"-r{input_file}", f"-m{model}", f"-o{output_file}"
    )

    embedding = _load_embedding(output_file)
    assert embedding.source == "OpenAI"
    assert embedding.text in input_file.read_text()
    assert len(embedding.embedding) == 1536
    assert model == embedding.model
