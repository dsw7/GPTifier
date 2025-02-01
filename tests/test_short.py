from unittest import TestCase
from helpers import run_process


class TestShort(TestCase):

    def test_help(self):
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["short", option])
                proc.assert_success()
                self.assertIn("Synopsis", proc.stdout)

    def test_short_prompt(self):
        prompt = '"What is 2 + 2? Format the result as follows: >>>{result}<<<"'

        for option in ["-p", "--prompt="]:
            with self.subTest(option=option):
                proc = run_process(["short", option + prompt])
                proc.assert_success()
                self.assertIn(">>>4<<<", proc.stdout)

    def test_short_raw(self):
        prompt = '"What is 2 + 2?"'

        for option in ["-r", "--raw"]:
            with self.subTest(option=option):
                proc = run_process(["short", f"--prompt={prompt}", option])
                proc.assert_success()
                proc.load_stdout_to_json()

    def test_missing_prompt(self):
        proc = run_process("short")
        proc.assert_failure()
        self.assertIn("Prompt is empty", proc.stderr)
