from dataclasses import dataclass
from json import loads
from pathlib import Path
from tempfile import NamedTemporaryFile, gettempdir
from time import time
from typing import Generator
import pytest
import utils


DUMMY_PROMPT_1 = "What is 1 + 1? Format the result as follows: >>>{result}<<<"
DUMMY_PROMPT_2 = "What is 1 + 1?"


@dataclass
class _Response:
    created: int
    input_: str
    input_tokens: int
    model: str
    output: str
    output_tokens: int
    rtt: float
    source: str


def _load_content(json_file: str) -> _Response:
    with open(json_file) as f:
        contents = loads(f.read())

    return _Response(
        created=contents["created"],
        input_=contents["input"],
        input_tokens=contents["input_tokens"],
        model=contents["model"],
        output=contents["output"],
        output_tokens=contents["output_tokens"],
        rtt=contents["rtt"],
        source=contents["source"],
    )


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help(option: str) -> None:
    stdout = utils.assert_command_success("run", option)
    assert "Create a response according to a prompt." in stdout


@pytest.fixture
def inputfile() -> Generator[Path, None, None]:
    filename = Path.cwd() / "Inputfile"
    yield filename
    filename.unlink()


@pytest.mark.test_openai
def test_read_from_inputfile_openai(inputfile: Path) -> None:
    prompt = "What is 1 + 3? Format the result as follows: >>>{result}<<<"
    inputfile.write_text(prompt)

    with NamedTemporaryFile(dir=gettempdir()) as f:
        json_file = f.name
        utils.assert_command_success("run", "-t0", f"-o{json_file}")
        content = _load_content(json_file)
        assert content.output == ">>>4<<<"


@pytest.mark.test_ollama
def test_read_from_inputfile_ollama(inputfile: Path) -> None:
    prompt = "What is 1 + 3? Format the result as follows: >>>{result}<<<"
    inputfile.write_text(prompt)

    with NamedTemporaryFile(dir=gettempdir()) as f:
        json_file = f.name
        utils.assert_command_success("run", "-t0", f"-o{json_file}", "--use-local")
        content = _load_content(json_file)
        assert ">>>4<<<" in content.output


@pytest.mark.test_openai
def test_valid_response_json_openai() -> None:
    prompt = "What is 1 + 4? Format the result as follows: >>>{result}<<<"

    with NamedTemporaryFile(dir=gettempdir()) as f:
        utils.assert_command_success(
            "run", f"-p{prompt}", "-t0", f"-o{f.name}", "--model=gpt-4o"
        )
        content = _load_content(f.name)

        assert ">>>5<<<" in content.output
        assert content.input_ == prompt
        assert content.model == "gpt-4o"
        assert content.source == "OpenAI"

        diff_created = abs(content.created - int(time()))
        assert diff_created <= 2.0, "Creation times are not within 2 seconds"


@pytest.mark.test_ollama
def test_valid_response_json_ollama() -> None:
    prompt = "What is 1 + 4? Format the result as follows: >>>{result}<<<"

    with NamedTemporaryFile(dir=gettempdir()) as f:
        utils.assert_command_success(
            "run",
            f"-p{prompt}",
            "-t0",
            f"-o{f.name}",
            "--use-local",
            "--model=gemma3:latest",
        )
        content = _load_content(f.name)

        assert ">>>5<<<" in content.output
        assert content.input_ == prompt
        assert content.model == "gemma3:latest"
        assert content.source == "Ollama"


@pytest.mark.test_openai
def test_read_from_prompt_file_openai() -> None:
    prompt = Path(__file__).resolve().parent / "test_run" / "prompt_basic.txt"

    with NamedTemporaryFile(dir=gettempdir()) as f:
        json_file = f.name
        utils.assert_command_success("run", f"-r{prompt}", "-t0", f"-o{json_file}")
        content = _load_content(json_file)
        assert ">>>8<<<" in content.output


@pytest.mark.test_ollama
def test_read_from_prompt_file_ollama() -> None:
    prompt = Path(__file__).resolve().parent / "test_run" / "prompt_basic.txt"

    with NamedTemporaryFile(dir=gettempdir()) as f:
        json_file = f.name
        utils.assert_command_success(
            "run", f"-r{prompt}", "-t0", f"-o{json_file}", "--use-local"
        )
        content = _load_content(json_file)
        assert ">>>8<<<" in content.output


@pytest.mark.test_openai
def test_write_to_stdout_openai() -> None:
    stdout = utils.assert_command_success("run", f"-p'{DUMMY_PROMPT_1}'")
    assert ">>>2<<<" in stdout


@pytest.mark.test_ollama
def test_write_to_stdout_ollama() -> None:
    stdout = utils.assert_command_success("run", f"-p'{DUMMY_PROMPT_1}'", "-l")
    assert ">>>2<<<" in stdout


def test_invalid_temp() -> None:
    stderr = utils.assert_command_failure("run", f"-p'{DUMMY_PROMPT_1}'", "-tfoobar")
    assert "Failed to convert 'foobar' to float" in stderr


def test_empty_temp() -> None:
    stderr = utils.assert_command_failure(
        "run", "-p'A foo that bars?'", "--temperature="
    )
    assert "Empty temperature" in stderr


def test_missing_prompt_file() -> None:
    stderr = utils.assert_command_failure("run", "--read-from-file=/tmp/yU8nnkRs.txt")
    assert "Unable to open '/tmp/yU8nnkRs.txt'" in stderr


def test_empty_prompt() -> None:
    stderr = utils.assert_command_failure("run", "--prompt=")
    assert "Prompt is empty" in stderr


def test_empty_prompt_file() -> None:
    stderr = utils.assert_command_failure("run", "--read-from-file=")
    assert "Empty prompt filename" in stderr


def test_invalid_output_file_location() -> None:
    stderr = utils.assert_command_failure(
        "run", f"--prompt='{DUMMY_PROMPT_1}'", "--file=/tmp/a/b/c"
    )
    assert "Unable to open '/tmp/a/b/c'" in stderr


def test_empty_output_file() -> None:
    stderr = utils.assert_command_failure("run", "--prompt='What is 7 + 2?'", "--file=")
    assert "No filename provided" in stderr


def test_empty_model() -> None:
    stderr = utils.assert_command_failure("run", "-p'foobar'", "--model=")
    assert "Model is empty" in stderr


@pytest.mark.parametrize(
    "model",
    [
        "codex-mini-latest",
        "gpt-3.5-turbo",
        "gpt-4",
        "gpt-4.1",
        "gpt-4.1-mini",
        "gpt-4.1-nano",
        "gpt-4o",
        "gpt-4o-mini",
        "o1",
        "o1-pro",
        "o3-mini",
        "o4-mini",
    ],
)
def test_misc_valid_models(model: str) -> None:
    prompt = "What is 1 + 1?"
    utils.assert_command_success("run", f"-p'{prompt}'", f"-m{model}")


def test_non_existent_model() -> None:
    stderr = utils.assert_command_failure("run", f"-p'{DUMMY_PROMPT_2}'", "-mfoobar")
    assert "The requested model 'foobar' does not exist.\nCannot proceed" in stderr


@pytest.mark.parametrize(
    "model",
    [
        "gpt-4o-transcribe",
        "gpt-image-1",
    ],
)
def test_wrong_endpoint_model_not_supported(model: str) -> None:
    stderr = utils.assert_command_failure("run", f"-p'{DUMMY_PROMPT_2}'", f"-m{model}")
    assert (
        f"The requested model '{model}' is not supported with the Responses API.\nCannot proceed\n"
        in stderr
    )


@pytest.mark.parametrize(
    "model",
    [
        "dall-e-2",
        "dall-e-3",
        "davinci-002",
        "text-embedding-3-large",
        "text-embedding-3-small",
    ],
)
def test_wrong_endpoint_model_not_found(model: str) -> None:
    stderr = utils.assert_command_failure("run", f"-p'{DUMMY_PROMPT_2}'", f"-m{model}")
    assert f"The requested model, '{model}' was not found.\nCannot proceed\n" in stderr


@pytest.mark.parametrize("model", ["gpt-5", "gpt-5-mini", "gpt-5-codex", "o3"])
def test_unverified_organization_error(model: str) -> None:
    stderr = utils.assert_command_failure("run", f"-p'{DUMMY_PROMPT_2}'", f"-m{model}")
    assert f"Your organization must be verified to use the model '{model}'." in stderr


def test_wrong_endpoint_deep_research() -> None:
    model = "o4-mini-deep-research"
    stderr = utils.assert_command_failure("run", f"-p'{DUMMY_PROMPT_2}'", f"-m{model}")
    assert (
        "Deep research models require at least one of 'web_search_preview', 'mcp', or 'file_search' tools.\nCannot proceed\n"
        in stderr
    )


@pytest.mark.parametrize("model", ["sora-2", "sora-2-pro"])
def test_sora_2(model: str) -> None:
    stderr = utils.assert_command_failure("run", f"-p'{DUMMY_PROMPT_2}'", f"-m{model}")
    assert f"Model not found {model}" in stderr
