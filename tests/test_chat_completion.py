from json import loads
from pathlib import Path
from tempfile import NamedTemporaryFile, gettempdir
from unittest import TestCase
from .helpers import run_process

Prompt = "What is 3 + 5? Format the result as follows: >>>{result}<<<"


def load_content(json_file: str) -> str:
    with open(json_file) as f:
        contents = loads(f.read())

    result: str = contents["completion"]
    return result


class TestChatCompletionReadFromInputfile(TestCase):
    def setUp(self) -> None:
        self.filename = Path.cwd() / "Inputfile"
        self.filename.write_text(Prompt)

    def tearDown(self) -> None:
        self.filename.unlink()

    def test_read_from_inputfile(self) -> None:
        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            proc = run_process(["run", "-t0", f"-d{json_file}", "-u"])
            proc.assert_success()
            self.assertEqual(load_content(json_file), ">>>8<<<")


class TestChatCompletion(TestCase):
    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["run", option])
                proc.assert_success()
                self.assertIn("Synopsis", proc.stdout)

    def test_read_from_command_line(self) -> None:
        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            proc = run_process(["run", f"-p'{Prompt}'", "-t0", f"-d{json_file}", "-u"])
            proc.assert_success()
            self.assertEqual(load_content(json_file), ">>>8<<<")

    def test_read_from_file(self) -> None:
        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            prompt = Path(__file__).resolve().parent / "prompt_basic.txt"
            proc = run_process(["run", f"-r{prompt}", "-t0", f"-d{json_file}", "-u"])
            proc.assert_success()
            self.assertEqual(load_content(json_file), ">>>8<<<")

    def test_invalid_temp(self) -> None:
        proc = run_process(["run", f"-p'{Prompt}'", "-tfoobar", "-u"])
        proc.assert_failure()
        self.assertIn("Failed to convert 'foobar' to float", proc.stderr)

    def test_missing_prompt_file(self) -> None:
        proc = run_process(["run", "--read-from-file=/tmp/yU8nnkRs.txt", "-u"])
        proc.assert_failure()
        self.assertIn("Could not open file '/tmp/yU8nnkRs.txt'", proc.stderr)

    def test_invalid_dump_location(self) -> None:
        proc = run_process(["run", f"--prompt='{Prompt}'", "--dump=/tmp/a/b/c", "-u"])
        proc.assert_failure()
        self.assertIn("Unable to open '/tmp/a/b/c'", proc.stderr)

    def test_invalid_model(self) -> None:
        proc = run_process(["run", f"-p'{Prompt}'", "-mfoobar", "-u"])
        proc.assert_failure()
        self.assertIn(
            "The model `foobar` does not exist or you do not have access to it.",
            proc.stderr,
        )

    def test_out_of_range_temp(self) -> None:
        for temp in [-2.5, 2.5]:
            with self.subTest(temp=temp):
                proc = run_process(["run", f"-p'{Prompt}'", f"-t{temp}", "-u"])
                proc.assert_failure()
                self.assertIn("Temperature must be between 0 and 2", proc.stderr)
