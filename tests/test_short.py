from unittest import TestCase
from .helpers import run_process


class TestShort(TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["short", option])
                proc.assert_success()
                self.assertIn("Synopsis", proc.stdout)

    prompt = '"What is 2 + 2? Format the result as follows: >>>{result}<<<"'

    def test_short_prompt(self) -> None:
        proc = run_process(["short", self.prompt])
        proc.assert_success()
        self.assertIn(">>>4<<<", proc.stdout)

    def test_short_raw_json(self) -> None:
        for option in ["-j", "--json"]:
            with self.subTest(option=option):
                proc = run_process(["short", option, self.prompt])
                proc.assert_success()

                results = proc.load_stdout_to_json()
                self.assertIn(">>>4<<<", results["choices"][0]["message"]["content"])

    def test_missing_prompt(self) -> None:
        proc = run_process("short")
        proc.assert_failure()
        self.assertIn("Prompt is empty", proc.stderr)
