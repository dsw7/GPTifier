from .extended_testcase import TestCaseExtended


class TestEdit(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("edit", option)
                self.assertIn(
                    "Edit one or more files according to a prompt.", proc.stdout
                )

    def test_missing_prompt_file(self) -> None:
        proc = self.assertFailure("edit")
        self.assertIn("No prompt file provided. Cannot proceed", proc.stderr)

    def test_missing_edit_file(self) -> None:
        proc = self.assertFailure("edit", "prompt")
        self.assertIn("No files to edit provided", proc.stderr)
