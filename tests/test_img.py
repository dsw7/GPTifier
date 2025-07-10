from .extended_testcase import TestCaseExtended


class TestImg(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("img", option)
                self.assertIn("Generate an image from a prompt", proc.stdout)

    def test_missing_prompt_file(self) -> None:
        proc = self.assertFailure("img")
        self.assertIn("No prompt file provided. Cannot proceed", proc.stderr)
