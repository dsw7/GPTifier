from datetime import datetime
from unittest import TestCase
from .helpers import run_process


class TestVersion(TestCase):

    def test_version_version(self) -> None:
        for option in ["-v", "--version"]:
            with self.subTest(option=option):
                proc = run_process(option)
                proc.assert_success()

                stdout = proc.load_stdout_to_json()
                self.assertIn("version", stdout)

    def test_version_build_type(self) -> None:
        for option in ["-v", "--version"]:
            with self.subTest(option=option):
                proc = run_process(option)
                proc.assert_success()

                stdout = proc.load_stdout_to_json()
                self.assertIn("build_type", stdout)
                self.assertEqual(stdout["build_type"], "Testing")

    def test_version_build_date(self) -> None:
        for option in ["-v", "--version"]:
            with self.subTest(option=option):
                proc = run_process(option)
                proc.assert_success()

                stdout = proc.load_stdout_to_json()
                self.assertIn("build_date", stdout)

                build_date = datetime.strptime(
                    stdout["build_date"], "%Y-%m-%dT%H:%M:%S"
                )
                now = datetime.now()

                self.assertEqual(build_date.day, now.day)
                self.assertEqual(build_date.month, now.month)
                self.assertEqual(build_date.year, now.year)
