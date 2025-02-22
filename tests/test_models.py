from unittest import TestCase
from .helpers import run_process


class TestModels(TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["models", option])

                proc.assert_success()
                self.assertIn("Synopsis", proc.stdout)

    pattern = r"Creation time\s+Owner\s+Model ID"

    def test_models_openai(self) -> None:
        proc = run_process("models")
        proc.assert_success()
        self.assertRegex(proc.stdout, self.pattern)

    def test_models_user(self) -> None:
        for option in ["-u", "--user"]:
            with self.subTest(option=option):
                proc = run_process(["models", option])
                proc.assert_success()
                self.assertRegex(proc.stdout, self.pattern)

    def test_models_raw_json(self) -> None:
        for option in ["-j", "--json"]:
            with self.subTest(option=option):
                proc = run_process(["models", option])
                proc.assert_success()
                proc.load_stdout_to_json()
