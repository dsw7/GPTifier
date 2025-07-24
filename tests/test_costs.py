from unittest import skipIf
from os import environ
from .extended_testcase import TestCaseExtended


@skipIf(
    "OPENAI_ADMIN_KEY" not in environ, "OPENAI_ADMIN_KEY does not exist in environment"
)
class TestCosts(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("costs", option)
                self.assertIn("Get OpenAI usage details", proc.stdout)

    def test_costs(self) -> None:
        proc = self.assertSuccess("costs", "--days=4")
        self.assertIn("Overall usage", proc.stdout)

    def test_costs_count(self) -> None:
        proc = self.assertSuccess("costs", "--json", "--days=3")
        json = proc.load_stdout_to_json()
        self.assertEqual(len(json["data"]), 3)

    def test_costs_raw_json(self) -> None:
        for option in ["-j", "--json"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("costs", option, "--days=3")
                proc.load_stdout_to_json()

    def test_costs_invalid_days_empty(self) -> None:
        proc = self.assertFailure("costs", "--days=")
        self.assertIn("Days argument is empty", proc.stderr)

    def test_costs_invalid_days_low(self) -> None:
        proc = self.assertFailure("costs", "--days=-1")
        self.assertIn("Days must be greater than 0", proc.stderr)

    def test_costs_invalid_days_high(self) -> None:
        proc = self.assertFailure("costs", "--days=180")
        self.assertIn("Days cannot be greater than 60", proc.stderr)

    def test_costs_invalid_days_stoi(self) -> None:
        proc = self.assertFailure("costs", "--days=abc")
        self.assertIn("Failed to convert 'abc' to int", proc.stderr)
