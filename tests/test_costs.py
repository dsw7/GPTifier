from unittest import TestCase
from .helpers import run_process


class TestCosts(TestCase):

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

    def test_costs_raw(self) -> None:
        for option in ["-r", "--raw"]:
            with self.subTest(option=option):
                proc = run_process(["costs", option])
                proc.assert_success()
                proc.load_stdout_to_json()

    def test_costs_invalid_days(self) -> None:
        proc = run_process(["costs", "--days=-1"])
        proc.assert_failure()
        self.assertIn("Days must be greater than 0", proc.stderr)
