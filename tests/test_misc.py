from datetime import datetime
from unittest import skipIf
from .extended_testcase import TestCaseExtended, is_memory_test


class TestRootHelp(TestCaseExtended):

    def test_root_help_long(self) -> None:
        self.assertSuccess("--help")

    def test_root_help_short(self) -> None:
        self.assertSuccess("-h")

    def test_root_help_empty_argv(self) -> None:
        # i.e. running just $ gpt
        proc = self.assertFailure()
        self.assertIn("A command line OpenAI toolkit.", proc.stdout.strip())

    def test_copyright(self) -> None:
        proc = self.assertSuccess("--help")
        self.assertIn(
            f"-- Copyright (C) 2023-{datetime.now().year} by David Weber",
            proc.stdout.strip(),
        )


class TestUnknownCommand(TestCaseExtended):

    def test_unknown_command(self) -> None:
        proc = self.assertFailure("foobar")
        self.assertEqual(
            proc.stderr.strip(), "Received unknown command. Re-run with -h or --help"
        )


class TestCatchMemoryLeak(TestCaseExtended):

    @skipIf(
        not is_memory_test(),
        "Test will not raise AssertionError if running outside of Valgrind context",
    )
    def test_catch_memory_leak(self) -> None:
        # Test that our custom assertSuccess catches a fake memory leak

        with self.assertRaises(AssertionError):
            proc = self.assertSuccess("test", "leak")
            self.assertEqual(proc.stdout.strip(), "5")


class TestCurlHandleReusability(TestCaseExtended):

    def test_reuse_curl_handle(self) -> None:
        # Test that curl handle can be reused in GPTifier's network layer

        proc = self.assertSuccess("test", "ccc")
        self.assertDictEqual(
            proc.load_stdout_to_json(),
            {"result_1": ">>>10<<<", "result_2": ">>>10<<<", "result_3": ">>>10<<<"},
        )
