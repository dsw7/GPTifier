from json import loads
from pathlib import Path
import subprocess
from pytest import fail


def test_basic_memory(tempdir: Path) -> None:
    json_file = tempdir / "test_basic_memory.json"

    command = [
        "valgrind",
        "--error-exitcode=1",
        "--leak-check=full",
        "build/gpt",
        "-u",
        "-p'What is 3 + 5? Format the result as follows: >>>{result}<<<'",
        "-t0",
        f"-d{json_file}",
    ]

    try:
        subprocess.run(
            command, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, check=True
        )
    except subprocess.CalledProcessError as exc:
        fail(exc.stderr.decode())

    with json_file.open() as f_json:
        data = loads(f_json.read())
        assert data["choices"][0]["message"]["content"] == ">>>8<<<"
