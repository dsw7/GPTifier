from json import loads
from pathlib import Path
from subprocess import run, CalledProcessError, PIPE, DEVNULL
from tempfile import gettempdir
from pytest import fail


def test_binary() -> None:
    json_file = Path(gettempdir()) / "gptifier_test.json"

    command = [
        "gpt",
        "--prompt='What is 3 + 5? Format the result as follows: >>>{result}<<<'",
        f"--dump={json_file}",
    ]

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())

    with json_file.open() as f:
        data = loads(f.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"
