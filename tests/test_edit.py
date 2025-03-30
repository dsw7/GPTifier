from pathlib import Path
from .extended_testcase import TestCaseExtended


INPUT_CODE = (
    "from .myfunc import myfunc\n"
    "\n"
    "def main() -> None:\n"
    '    myfunc(["a", "b", "c"])\n'
    "\n"
    'if __name__ == "__main__":\n'
    "    main()\n"
)
PROMPT = (
    "Given we have a function call of form:\n"
    "```\n"
    "proc = myfunc([A, B, C])\n"
    "```\n"
    "Replace this call with a call of form:\n"
    "```\n"
    "proc = myfunc(A, B, C)\n"
    "```\n"
)
OUTPUT_CODE = (
    "from .myfunc import myfunc\n"
    "\n"
    "def main() -> None:\n"
    '    myfunc("a", "b", "c")\n'
    "\n"
    'if __name__ == "__main__":\n'
    "    main()\n"
)


class TestEdit(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("edit", option)
                self.assertIn(
                    "Edit one or more files according to a prompt.", proc.stdout
                )

    def test_missing_prompt_file_option(self) -> None:
        proc = self.assertFailure("edit")
        self.assertIn("No prompt file provided. Cannot proceed", proc.stderr)

    def test_missing_output_file_argument(self) -> None:
        proc = self.assertFailure("edit", "-pprompt")
        self.assertIn("No file to edit provided. Cannot proceed", proc.stderr)

    def test_missing_prompt_file(self) -> None:
        for option in ["-pfoobar.txt", "--prompt=foobar.txt"]:
            with self.subTest(option=option):
                proc = self.assertFailure("edit", option, "test.cpp")
                self.assertIn("Unable to open 'foobar.txt'", proc.stderr)


class TestEditFile(TestCaseExtended):

    def setUp(self) -> None:
        self.input_file = Path("/tmp/foo.py")
        self.output_file = Path("/tmp/bar.py")
        self.prompt_file = Path("/tmp/prompt")
        self.input_file.write_text(INPUT_CODE)
        self.prompt_file.write_text(PROMPT)

    def tearDown(self) -> None:
        self.input_file.unlink()
        self.prompt_file.unlink()

        if self.output_file.exists():
            self.output_file.unlink()

    def test_missing_input_file_argument(self) -> None:
        proc = self.assertFailure("edit", f"-p{self.prompt_file}", "test.cpp")
        self.assertIn("Unable to open 'test.cpp'", proc.stderr)

    def test_invalid_model(self) -> None:
        for option in ["-mabc", "--model=abc"]:
            with self.subTest(option=option):
                proc = self.assertFailure(
                    "edit", option, f"-p{self.prompt_file}", str(self.input_file)
                )
                self.assertIn(
                    "The model `abc` does not exist or you do not have access to it.",
                    proc.stderr,
                )

    def test_debug_flag(self) -> None:
        proc = self.assertSuccess(
            "edit", f"-p{self.prompt_file}", str(self.input_file), "--debug"
        )
        self.assertIn("The prompt was:", proc.stdout)
        self.assertIn("The completion was:", proc.stdout)
        self.assertFalse(self.output_file.exists())

    def test_write_to_stdout(self) -> None:
        proc = self.assertSuccess("edit", f"-p{self.prompt_file}", str(self.input_file))
        self.assertIn(OUTPUT_CODE, proc.stdout)

    def test_write_to_file(self) -> None:
        self.assertSuccess(
            "edit",
            str(self.input_file),
            f"-p{self.prompt_file}",
            f"-o{self.output_file}",
        )
        self.assertTrue(self.output_file.exists())
        self.assertIn(self.output_file.read_text(), OUTPUT_CODE)
