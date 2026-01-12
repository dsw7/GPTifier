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


def test_non_existent_model() -> None:
    stderr = utils.assert_command_failure("embed", "-i'What is 3 + 5?'", "-mfoobar")
    assert (
        "The model `foobar` does not exist or you do not have access to it." in stderr
    )


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
def test_wrong_models(model: str) -> None:
    stderr = utils.assert_command_failure(
        "embed", "--input='A foo that bars'", f"-m{model}"
    )
    assert "You are not allowed to generate embeddings from this model" in stderr


@dataclass
class _Embedding:
    embedding: list[float]
    model: str
    text: str


def _load_embedding(results_file: Path) -> _Embedding:
    with results_file.open() as f:
        data = loads(f.read())

    return _Embedding(
        model=data["model"], text=data["input"], embedding=data["embedding"]
    )


def _get_cosine_similarity(left: _Embedding, right: _Embedding) -> float:
    dot_p: float = 0

    for l, r in zip(left.embedding, right.embedding):
        dot_p += l * r

    mag_l: float = (sum(i**2 for i in left.embedding)) ** 0.5
    mag_r: float = (sum(i**2 for i in right.embedding)) ** 0.5

    return dot_p / (mag_l * mag_r)


@pytest.fixture
def files_parallel() -> Generator[tuple[Path, Path], None, None]:
    input_file = Path(__file__).resolve().parent / "test_embed" / "lorem.txt"
    output_file = Path(gettempdir()) / "result.gpt"
    yield input_file, output_file

    if output_file.exists():
        output_file.unlink()


@pytest.fixture
def files_orthogonal() -> Generator[tuple[Path, Path], None, None]:
    filename_1 = Path(gettempdir()) / "result_1.gpt"
    filename_2 = Path(gettempdir()) / "result_2.gpt"

    yield filename_1, filename_2

    if filename_1.exists():
        filename_1.unlink()

    if filename_2.exists():
        filename_2.unlink()


def test_compute_cosine_similarities_parallel(
    files_parallel: tuple[Path, Path],
) -> None:
    input_file, output_file = files_parallel
    model = "text-embedding-3-small"
    utils.assert_command_success(
        "embed", f"-r{input_file}", f"-m{model}", f"-o{output_file}"
    )

    embedding = _load_embedding(output_file)

    assert model == embedding.model
    assert embedding.text in input_file.read_text()

    assert len(embedding.embedding) == 1536  # text-embedding-ada-002 dimension
    pytest.approx(_get_cosine_similarity(embedding, embedding), 1.00)


def test_compute_cosine_similarities_orthogonal(
    files_orthogonal: tuple[Path, Path],
) -> None:
    filename_1, filename_2 = files_orthogonal
    model = "text-embedding-ada-002"

    text_1 = "The cat meowed softly."
    utils.assert_command_success(
        "embed", f"-i{text_1}", f"-m{model}", f"-o{filename_1}"
    )
    embedding_1 = _load_embedding(filename_1)

    text_2 = "Quantum physics is fascinating."
    utils.assert_command_success(
        "embed", f"-i{text_2}", f"-m{model}", f"-o{filename_2}"
    )
    embedding_2 = _load_embedding(filename_2)

    assert 0.6 <= _get_cosine_similarity(embedding_1, embedding_2) <= 0.8
