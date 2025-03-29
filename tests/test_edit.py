from .extended_testcase import TestCaseExtended


class TestEdit(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("edit", option)
                self.assertIn(
                    "Edit one or more files according to a prompt.", proc.stdout
                )
