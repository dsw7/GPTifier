from unittest import TestCase
from .helpers import run_process


class TestModels(TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["models", option])

                proc.assert_success()
                self.assertIn("Synopsis", proc.stdout)

    def test_models(self) -> None:
        proc = run_process("models")
        proc.assert_success()

        models = proc.stdout.split("\n")
        assert len(models) > 1

    def test_models_raw(self) -> None:
        for option in ["-r", "--raw"]:
            with self.subTest(option=option):
                proc = run_process(["models", option])
                proc.assert_success()
                proc.load_stdout_to_json()
