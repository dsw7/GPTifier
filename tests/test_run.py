from dataclasses import dataclass
from functools import cache
from json import loads
from pathlib import Path
from tempfile import NamedTemporaryFile, gettempdir
from unittest import TestCase
from .helpers import run_process


@dataclass
class Pair:
    completion: str
    prompt: str


@dataclass
class Completion:
    completion: str
    completion_tokens: int
    created: int
    model: str
    prompt: str
    prompt_tokens: int
    rtt: float


@cache
def get_prompt_completion_pair(x: int = 1) -> Pair:
    return Pair(
        prompt=f"What is 1 + {x}? Format the result as follows: >>>{{result}}<<<",
        completion=f">>>{1 + x}<<<",
    )


def load_content(json_file: str) -> Completion:
    with open(json_file) as f:
        contents = loads(f.read())

    return Completion(
        completion=contents["completion"],
        completion_tokens=contents["completion_tokens"],
        created=contents["created"],
        model=contents["model"],
        prompt=contents["prompt"],
        prompt_tokens=contents["prompt_tokens"],
        rtt=contents["rtt"],
    )


class TestChatCompletionReadFromInputfile(TestCase):
    def setUp(self) -> None:
        self.filename = Path.cwd() / "Inputfile"

    def tearDown(self) -> None:
        self.filename.unlink()

    def test_read_from_inputfile(self) -> None:
        pair = get_prompt_completion_pair(3)
        self.filename.write_text(pair.prompt)

        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            proc = run_process(["run", "-t0", f"-o{json_file}", "-u"])
            proc.assert_success()
            completion = load_content(json_file)
            self.assertEqual(completion.completion, pair.completion)


class TestChatCompletion(TestCase):
    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["run", option])
                proc.assert_success()
                self.assertIn("Create a chat completion.", proc.stdout)

    def test_read_from_command_line(self) -> None:
        pair = get_prompt_completion_pair(5)

        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            proc = run_process(
                ["run", f"-p'{pair.prompt}'", "-t0", f"-o{json_file}", "-u"]
            )
            proc.assert_success()
            completion = load_content(json_file)
            self.assertEqual(completion.completion, pair.completion)

    def test_read_from_file(self) -> None:
        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            prompt = Path(__file__).resolve().parent / "prompt_basic.txt"
            proc = run_process(["run", f"-r{prompt}", "-t0", f"-o{json_file}", "-u"])
            proc.assert_success()
            completion = load_content(json_file)
            self.assertEqual(completion.completion, ">>>8<<<")

    def test_invalid_temp(self) -> None:
        prompt = get_prompt_completion_pair().prompt

        proc = run_process(["run", f"-p'{prompt}'", "-tfoobar", "-u"])
        proc.assert_failure()
        self.assertIn("Failed to convert 'foobar' to float", proc.stderr)

    def test_missing_prompt_file(self) -> None:
        proc = run_process(["run", "--read-from-file=/tmp/yU8nnkRs.txt", "-u"])
        proc.assert_failure()
        self.assertIn("Could not open file '/tmp/yU8nnkRs.txt'", proc.stderr)

    def test_invalid_dump_location(self) -> None:
        prompt = get_prompt_completion_pair().prompt

        proc = run_process(["run", f"--prompt='{prompt}'", "--file=/tmp/a/b/c", "-u"])
        proc.assert_failure()
        self.assertIn("Unable to open '/tmp/a/b/c'", proc.stderr)

    def test_invalid_model(self) -> None:
        prompt = get_prompt_completion_pair().prompt

        proc = run_process(["run", f"-p'{prompt}'", "-mfoobar", "-u"])
        proc.assert_failure()
        self.assertIn(
            "The model `foobar` does not exist or you do not have access to it.",
            proc.stderr,
        )

    def test_out_of_range_temp(self) -> None:
        prompt = get_prompt_completion_pair().prompt

        for temp in [-2.5, 2.5]:
            with self.subTest(temp=temp):
                proc = run_process(["run", f"-p'{prompt}'", f"-t{temp}", "-u"])
                proc.assert_failure()
                self.assertIn("Temperature must be between 0 and 2", proc.stderr)
