from json import loads
from os import EX_OK
from pathlib import Path
from subprocess import run, DEVNULL, PIPE, CalledProcessError
from pytest import mark, fail


def test_read_from_file(tempdir: Path) -> None:
    json_file = tempdir / "test_read_from_file.json"

    prompt = tempdir / "test_read_from_file.txt"
    prompt.write_text("What is 3 + 5? Format the result as follows: >>>{result}<<<")

    command = ["build/gpt", "-u", f"-r{prompt}", "-t0", f"-d{json_file}"]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())

    with json_file.open() as f_json:
        data = loads(f_json.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"


def test_missing_prompt_file(capfd) -> None:
    process = run(["build/gpt", "--read-from-file=/tmp/yU8nnkRs.txt"], stdout=DEVNULL)
    assert process.returncode != EX_OK

    cap = capfd.readouterr()
    assert cap.err.strip() == "Could not open file '/tmp/yU8nnkRs.txt'"


def test_invalid_dump_loc(capfd) -> None:
    process = run(
        ["build/gpt", "--prompt='What is 3 + 5?'", "--dump=/tmp/a/b/c"],
        stdout=DEVNULL,
    )
    assert process.returncode != EX_OK

    cap = capfd.readouterr()
    assert cap.err.strip() == "Unable to open '/tmp/a/b/c'"


@mark.parametrize(
    "model, result, valid_model",
    [
        ("gpt-3.5-turbo-0301", "gpt-3.5-turbo-0301", True),
        ("gpt-3.5-turbo-0302", "The model `gpt-3.5-turbo-0302` does not exist", False),
    ],
)
def test_model(model: str, result: str, valid_model: bool, tempdir: Path) -> None:
    json_file = tempdir / "test_model.json"

    command = ["build/gpt", "-u", "-p'What is 3 + 5?'", f"-m{model}", f"-d{json_file}"]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())

    with json_file.open() as f_json:
        data = loads(f_json.read())

    if valid_model:
        assert data["model"] == result
    else:
        assert data["error"]["message"] == result
