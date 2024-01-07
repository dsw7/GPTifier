from json import loads
from subprocess import run, DEVNULL, PIPE, CalledProcessError
from pytest import fail


def test_basic(json_file: str, command: list[str]) -> None:
    command.extend(
        [
            "-p'What is 3 + 5? Format the result as follows: >>>{result}<<<'",
            "-t0",
            f"-d{json_file}",
        ]
    )

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())

    with open(json_file) as f_json:
        data = loads(f_json.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"
