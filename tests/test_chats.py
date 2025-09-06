from .extended_testcase import TestCaseExtended


class TestChats(TestCaseExtended):
    def test_throw_deprecation_exception(self) -> None:
        proc = self.assertFailure("chats")
        self.assertIn("This command has been deprecated.", proc.stderr)
