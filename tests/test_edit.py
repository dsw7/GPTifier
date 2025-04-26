from pathlib import Path
from shutil import copy
import subprocess
from .extended_testcase import TestCaseExtended

PathToData = Path(__file__).resolve().parent / "test_edit"


class TestEdit(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("edit", option)
                self.assertIn(
                    "Edit code according to rules provided by file or command line option.",
                    proc.stdout,
                )

    def test_missing_instructions_file_option(self) -> None:
        proc = self.assertFailure("edit")
        self.assertIn(
            "No instructions file or rule provided. Cannot proceed", proc.stderr
        )

    def test_missing_output_file_argument(self) -> None:
        proc = self.assertFailure("edit", "-ifoobar")
        self.assertIn("No file to edit provided. Cannot proceed", proc.stderr)

    def test_missing_instructions_file(self) -> None:
        for option in ["-ifoobar.txt", "--instructions=foobar.txt"]:
            with self.subTest(option=option):
                proc = self.assertFailure("edit", option, "test.cpp")
                self.assertIn("Unable to open 'foobar.txt'", proc.stderr)


class TestEditFile(TestCaseExtended):

    def setUp(self) -> None:
        self.input_file = Path("/tmp/foo.py")
        self.output_file = Path("/tmp/bar.py")
        self.instructions_file = Path("/tmp/instructions.txt")

        copy(PathToData / "dummy.py", self.input_file)
        copy(PathToData / "instructions.txt", self.instructions_file)

    def tearDown(self) -> None:
        self.input_file.unlink()
        self.instructions_file.unlink()

        if self.output_file.exists():
            self.output_file.unlink()

    def test_missing_input_file_argument(self) -> None:
        proc = self.assertFailure("edit", f"-i{self.instructions_file}", "test.cpp")
        self.assertIn("Unable to open 'test.cpp'", proc.stderr)

    def test_invalid_model(self) -> None:
        for option in ["-mabc", "--model=abc"]:
            with self.subTest(option=option):
                proc = self.assertFailure(
                    "edit", option, f"-i{self.instructions_file}", str(self.input_file)
                )
                self.assertIn(
                    "The model `abc` does not exist or you do not have access to it.",
                    proc.stderr,
                )

    def test_debug_flag(self) -> None:
        proc = self.assertSuccess(
            "edit", f"-i{self.instructions_file}", str(self.input_file), "--debug"
        )
        self.assertIn("The prompt was:", proc.stdout)
        self.assertIn("The completion was:", proc.stdout)
        self.assertFalse(self.output_file.exists())

    def test_debug_flag_rule(self) -> None:
        instructions = self.instructions_file.read_text()
        proc = self.assertSuccess(
            "edit", f"-r{instructions}", str(self.input_file), "--debug"
        )
        self.assertIn("The prompt was:", proc.stdout)
        self.assertIn("The completion was:", proc.stdout)
        self.assertFalse(self.output_file.exists())

    def test_write_to_stdout(self) -> None:
        proc = self.assertSuccess(
            "edit", f"-i{self.instructions_file}", str(self.input_file)
        )
        self.assertIn("The updated code is:", proc.stdout)
        self.assertIn("A description of the changes:", proc.stdout)

    def test_write_to_stdout_rule(self) -> None:
        instructions = self.instructions_file.read_text()
        proc = self.assertSuccess("edit", f"-r{instructions}", str(self.input_file))
        self.assertIn("The updated code is:", proc.stdout)
        self.assertIn("A description of the changes:", proc.stdout)

    def test_write_to_file(self) -> None:
        self.assertSuccess(
            "edit",
            str(self.input_file),
            f"-i{self.instructions_file}",
            f"-o{self.output_file}",
        )
        process = subprocess.run(["python3", self.output_file], capture_output=True)
        self.assertEqual(process.returncode, 0)
        self.assertEqual(process.stdout.decode(), "6\n")

    def test_write_to_file_rule(self) -> None:
        instructions = self.instructions_file.read_text()
        self.assertSuccess(
            "edit",
            str(self.input_file),
            f"--rule={instructions}",
            f"-o{self.output_file}",
        )
        process = subprocess.run(["python3", self.output_file], capture_output=True)
        self.assertEqual(process.returncode, 0)
        self.assertEqual(process.stdout.decode(), "6\n")

    def test_overwrite_file(self) -> None:
        self.assertSuccess(
            "edit",
            str(self.input_file),
            f"-i{self.instructions_file}",
            f"-o{self.input_file}",
        )
        process = subprocess.run(["python3", self.input_file], capture_output=True)
        self.assertEqual(process.returncode, 0)
        self.assertEqual(process.stdout.decode(), "6\n")

    def test_overwrite_file_rule(self) -> None:
        instructions = self.instructions_file.read_text()
        self.assertSuccess(
            "edit",
            str(self.input_file),
            f"--rule={instructions}",
            f"-o{self.input_file}",
        )
        process = subprocess.run(["python3", self.input_file], capture_output=True)
        self.assertEqual(process.returncode, 0)
        self.assertEqual(process.stdout.decode(), "6\n")
