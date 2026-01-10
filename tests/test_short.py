from typing import Any
import pytest
import utils

PROMPT = '"What is 2 + 2? Format the result as follows: >>>{result}<<<"'
MODEL_OLLAMA = "gemma3:latest"
MODEL_OPENAI = "gpt-3.5-turbo"


@pytest.mark.parametrize("option", ["-h", "--help"])
def test_help(option: str) -> None:
    stdout = utils.assert_command_success("short", option)
    assert "Create a response but without threading or verbosity." in stdout


@pytest.mark.test_openai
def test_short_prompt_openai() -> None:
    stdout = utils.assert_command_success(
        "short", f"--model={MODEL_OPENAI}", "--temperature=1.00", PROMPT
    )
    assert ">>>4<<<" in stdout


@pytest.mark.test_ollama
def test_short_prompt_ollama() -> None:
    stdout = utils.assert_command_success(
        "short", "--use-local", f"--model={MODEL_OLLAMA}", PROMPT
    )
    assert ">>>4<<<" in stdout


@pytest.mark.test_openai
def test_invalid_model_openai() -> None:
    stderr = utils.assert_command_failure("short", "--model=foobar", PROMPT)
    assert "The requested model 'foobar' does not exist.\n" in stderr


@pytest.mark.test_ollama
def test_invalid_model_ollama() -> None:
    stderr = utils.assert_command_failure(
        "short", "--use-local", "--model=foobar", PROMPT
    )
    assert "model 'foobar' not found" in stderr


def _load_openai_output(output: Any) -> str:
    assert len(output) > 0, "Output is empty"
    text: str | None = None

    for item in output:
        if item["type"] != "message":
            continue

        if item["status"] != "completed":
            continue

        if item["content"][0]["type"] == "output_text":
            text = item["content"][0]["text"]
            break

    assert text is not None, "Could not find text in OpenAI output"
    # Can ignore this since assertion will break out of function if text is null
    return text


@pytest.mark.test_openai
@pytest.mark.parametrize("option", ["-j", "--json"])
def test_short_raw_json_openai(option: str) -> None:
    stdout = utils.assert_command_success(
        "short", option, f"--model={MODEL_OPENAI}", PROMPT
    )
    results = utils.load_stdout_to_json(stdout)
    assert ">>>4<<<" in _load_openai_output(results["output"])


@pytest.mark.test_ollama
@pytest.mark.parametrize("option", ["-j", "--json"])
def test_short_raw_json_ollama(option: str) -> None:
    stdout = utils.assert_command_success(
        "short", option, "--use-local", f"--model={MODEL_OLLAMA}", PROMPT
    )
    results = utils.load_stdout_to_json(stdout)
    assert ">>>4<<<" in results["response"]


@pytest.mark.test_openai
def test_empty_temp() -> None:
    stderr = utils.assert_command_failure("short", "--temperature=", PROMPT)
    assert "Empty temperature" in stderr


@pytest.mark.test_openai
def test_invalid_temp_stof() -> None:
    stderr = utils.assert_command_failure("short", "-tfoobar", PROMPT)
    assert "Failed to convert 'foobar' to float" in stderr


def test_missing_prompt() -> None:
    stderr = utils.assert_command_failure("short")
    assert "Prompt is empty" in stderr


def test_empty_prompt() -> None:
    stderr = utils.assert_command_failure("short", "")
    assert "Prompt is empty" in stderr
