from json import loads
from pathlib import Path
from subprocess import run, DEVNULL, PIPE, CalledProcessError
from pytest import mark, fail


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
