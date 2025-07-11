from .extended_testcase import TestCaseExtended


class TestEdit(TestCaseExtended):

    def test_throw_deprecation_exception(self) -> None:
        proc = self.assertFailure("edit")
        self.assertIn(
            "This command has been deprecated. See https://github.com/dsw7/FuncGraft for more information",
            proc.stderr,
        )
