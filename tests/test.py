from json import loads
from subprocess import run, DEVNULL, PIPE, CalledProcessError
from pytest import fail, mark


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


@mark.parametrize(
    "temp, result",
    [
        ("-2.5", "-2.5 is less than the minimum of 0 - 'temperature'"),
        ("2.5", "2.5 is greater than the maximum of 2 - 'temperature'"),
    ],
)
def test_invalid_temp(
    temp: str, result: str, json_file: str, command: list[str]
) -> None:
    command.extend(["-p'Running a test!'", f"-t{temp}", f"-d{json_file}"])

    try:
        run(command, stdout=DEVNULL, stderr=PIPE, check=True)
    except CalledProcessError as exc:
        fail(exc.stderr.decode())

    with open(json_file) as f_json:
        data = loads(f_json.read())
        assert data["error"]["message"] == result
