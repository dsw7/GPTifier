import unittest
from os import environ
from .helpers import run_process


@unittest.skipIf(
    "OPENAI_ADMIN_KEY" not in environ, "OPENAI_ADMIN_KEY does not exist in environment"
)
class TestCosts(unittest.TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["costs", option])

                proc.assert_success()
                self.assertIn("Synopsis", proc.stdout)

    def test_costs(self) -> None:
        proc = run_process(["costs", "--days=4"])
        proc.assert_success()

        costs = proc.stdout.split("\n")
        assert len(costs) > 1

    def test_costs_raw_json(self) -> None:
        for option in ["-j", "--json"]:
            with self.subTest(option=option):
                proc = run_process(["costs", option, "--days=3"])
                proc.assert_success()
                proc.load_stdout_to_json()

    def test_costs_invalid_days(self) -> None:
        proc = run_process(["costs", "--days=-1"])
        proc.assert_failure()
        self.assertIn("Days must be greater than 0", proc.stderr)
