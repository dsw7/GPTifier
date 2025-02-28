from unittest import TestCase
from .helpers import run_process


class TestChats(TestCase):
    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["chats", option])
                proc.assert_success()
                self.assertIn(
                    "Manage chat completions uploaded to OpenAI.", proc.stdout
                )

    def test_no_subcommand(self) -> None:
        proc = run_process("chats")
        proc.assert_success()


class TestChatsList(TestCase):
    def test_help_list(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["chats", "list", option])
                proc.assert_success()
                self.assertIn("List uploaded chat completions.", proc.stdout)

    def test_invalid_limit(self) -> None:
        for option in ["-l-1", "--limit=-1"]:
            with self.subTest(option=option):
                proc = run_process(["chats", "list", option])
                proc.assert_failure()
                self.assertIn("Limit must be greater than 0", proc.stderr)

    def test_invalid_limit_2(self) -> None:
        proc = run_process(["chats", "list", "--limit=abc"])
        proc.assert_failure()
        self.assertIn("Failed to convert 'abc' to int", proc.stderr)


class TestChatsDelete(TestCase):
    def test_help_delete(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["chats", "delete", option])
                proc.assert_success()
                self.assertIn("Delete an uploaded chat completion.", proc.stdout)

    def test_delete_missing_id(self) -> None:
        proc = run_process(["chats", "delete", "abc"])
        proc.assert_failure()
        self.assertIn(
            'Failed to delete chat completion with ID: abc. The error was: "Completion abc not found"',
            proc.stderr,
        )
