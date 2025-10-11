from dataclasses import dataclass
from functools import cache
from json import loads
from pathlib import Path
from tempfile import NamedTemporaryFile, gettempdir
from time import time
from .extended_testcase import TestCaseExtended


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


class TestChatCompletionReadFromInputfile(TestCaseExtended):
    def setUp(self) -> None:
        self.filename = Path.cwd() / "Inputfile"

    def tearDown(self) -> None:
        self.filename.unlink()

    def test_read_from_inputfile(self) -> None:
        pair = get_prompt_completion_pair(3)
        self.filename.write_text(pair.prompt)

        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            self.assertSuccess("run", "-t0", f"-o{json_file}")
            completion = load_content(json_file)
            self.assertEqual(completion.completion, pair.completion)


class TestChatCompletionJSON(TestCaseExtended):
    def setUp(self) -> None:
        self.pair = get_prompt_completion_pair(5)

        with NamedTemporaryFile(dir=gettempdir()) as f:
            t_start = time()
            # Yes... a hack :)
            self.assertSuccess("run", f"-p{self.pair.prompt}", "-t0", f"-o{f.name}")
            self.rtt = time() - t_start
            self.completion = load_content(f.name)

    def test_equal_prompt(self) -> None:
        self.assertEqual(self.completion.prompt, self.pair.prompt)

    def test_equal_completion(self) -> None:
        self.assertEqual(self.completion.completion, self.pair.completion)

    def test_prompt_tokens(self) -> None:
        self.assertEqual(self.completion.prompt_tokens, 26)

    def test_completion_tokens(self) -> None:
        self.assertEqual(self.completion.completion_tokens, 3)

    def test_approx_rtt(self) -> None:
        diff = abs(self.completion.rtt - self.rtt)
        self.assertLessEqual(diff, 0.25, "RTT times are not within 0.25 seconds")

    def test_approx_created(self) -> None:
        diff = abs(self.completion.created - int(time()))
        self.assertLessEqual(diff, 2.0, "Creation times are not within 2 seconds")


class TestChatCompletion(TestCaseExtended):
    prompt = "What is 1 + 1? Format the result as follows: >>>{result}<<<"

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("run", option)
                self.assertIn("Create a chat completion", proc.stdout)

    def test_read_from_file(self) -> None:
        prompt = Path(__file__).resolve().parent / "test_run" / "prompt_basic.txt"

        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            self.assertSuccess("run", f"-r{prompt}", "-t0", f"-o{json_file}")
            completion = load_content(json_file)
            self.assertEqual(completion.completion, ">>>8<<<")

    def test_write_to_stdout(self) -> None:
        proc = self.assertSuccess("run", f"-p'{self.prompt}'")
        self.assertIn(">>>2<<<", proc.stdout)

    def test_invalid_temp(self) -> None:
        proc = self.assertFailure("run", f"-p'{self.prompt}'", "-tfoobar")
        self.assertIn("Failed to convert 'foobar' to float", proc.stderr)

    def test_empty_temp(self) -> None:
        proc = self.assertFailure("run", "-p'A foo that bars?'", "--temperature=")
        self.assertIn("Empty temperature", proc.stderr)

    def test_missing_prompt_file(self) -> None:
        proc = self.assertFailure("run", "--read-from-file=/tmp/yU8nnkRs.txt")
        self.assertIn("Unable to open '/tmp/yU8nnkRs.txt'", proc.stderr)

    def test_empty_prompt(self) -> None:
        proc = self.assertFailure("run", "--prompt=")
        self.assertIn("No input text provided anywhere. Cannot proceed", proc.stderr)

    def test_empty_prompt_file(self) -> None:
        proc = self.assertFailure("run", "--read-from-file=")
        self.assertIn("Empty prompt filename", proc.stderr)

    def test_invalid_output_file_location(self) -> None:
        proc = self.assertFailure(
            "run", f"--prompt='{self.prompt}'", "--file=/tmp/a/b/c"
        )
        self.assertIn("Unable to open '/tmp/a/b/c'", proc.stderr)

    def test_empty_output_file(self) -> None:
        proc = self.assertFailure("run", "--prompt='What is 7 + 2?'", "--file=")
        self.assertIn("No filename provided", proc.stderr)

    def test_invalid_model(self) -> None:
        proc = self.assertFailure("run", f"-p'{self.prompt}'", "-mfoobar")
        self.assertIn(
            "The model `foobar` does not exist or you do not have access to it.",
            proc.stderr,
        )

    def test_empty_model(self) -> None:
        proc = self.assertFailure("run", "-p'foobar'", "--model=")
        self.assertIn("Model is empty", proc.stderr)

    def test_out_of_range_temp(self) -> None:
        for temp in [-2.5, 2.5]:
            with self.subTest(temp=temp):
                proc = self.assertFailure("run", f"-p'{self.prompt}'", f"-t{temp}")
                self.assertIn("Temperature must be between 0 and 2", proc.stderr)
