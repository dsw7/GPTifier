from time import sleep
from .extended_testcase import TestCaseExtended


class TestChats(TestCaseExtended):
    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("chats", option)
                self.assertIn(
                    "Manage chat completions uploaded to OpenAI.", proc.stdout
                )

    def test_no_subcommand(self) -> None:
        self.assertSuccess("chats")


class TestChatsList(TestCaseExtended):
    def test_help_list(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("chats", "list", option)
                self.assertIn("List uploaded chat completions.", proc.stdout)

    def test_invalid_limit(self) -> None:
        for option in ["-l-1", "--limit=-1"]:
            with self.subTest(option=option):
                proc = self.assertFailure("chats", "list", option)
                self.assertIn("Limit must be greater than 0", proc.stderr)

    def test_invalid_limit_2(self) -> None:
        proc = self.assertFailure("chats", "list", "--limit=abc")
        self.assertIn("Failed to convert 'abc' to int", proc.stderr)


class TestChatsDelete(TestCaseExtended):
    def test_help_delete(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("chats", "delete", option)
                self.assertIn("Delete an uploaded chat completion.", proc.stdout)

    def test_delete_missing_id(self) -> None:
        proc = self.assertFailure("chats", "delete", "abc")
        self.assertIn(
            'Failed to delete chat completion with ID: abc. The error was: "Completion abc not found"',
            proc.stderr,
        )


class TestRoundTrip(TestCaseExtended):

    def test_round_trip(self) -> None:
        proc = self.assertSuccess(
            "short", "What is 3 + 5?", "--json", "--store-completion"
        )
        results = proc.load_stdout_to_json()
        chat_compl_id = results["id"]

        count = 0
        found = False

        while count < 5:
            count += 1
            sleep(0.5)

            proc = self.assertSuccess("chats", "list", "--json")
            results = proc.load_stdout_to_json()

            if chat_compl_id in [i["id"] for i in results["data"]]:
                found = True
                break

        self.assertTrue(found)

        proc = self.assertSuccess("chats", "delete", chat_compl_id)
        self.assertIn(
            f"Success! Deleted chat completion with ID: {chat_compl_id}", proc.stdout
        )
