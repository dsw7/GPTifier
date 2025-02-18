from unittest import TestCase
from .helpers import run_process


class TestFiles(TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["files", option])
                proc.assert_success()
                self.assertIn("Synopsis", proc.stdout)

    def test_subcommand_help(self) -> None:
        for subcommand in ["list", "delete"]:
            for option in ["-h", "--help"]:
                with self.subTest(subcommand=subcommand, option=option):
                    proc = run_process(["files", subcommand, option])
                    proc.assert_success()
                    self.assertIn("Synopsis", proc.stdout)

    pattern = r"File ID\s+Filename\s+Creation time\s+Purpose"

    def test_files_list_default(self) -> None:
        proc = run_process("files")
        proc.assert_success()
        self.assertRegex(proc.stdout, self.pattern)

    def test_files_list(self) -> None:
        proc = run_process(["files", "list"])
        proc.assert_success()
        self.assertRegex(proc.stdout, self.pattern)

    def test_files_list_raw(self) -> None:
        for option in ["-r", "--raw"]:
            with self.subTest(option=option):
                proc = run_process(["files", "list", option])
                proc.assert_success()
                proc.load_stdout_to_json()

    def test_files_delete(self) -> None:
        proc = run_process(["files", "delete", "foobar"])
        proc.assert_failure()
        self.assertIn(
            'Failed to delete file with ID: foobar. The error was: "No such File object: foobar"\n'
            "One or more failures occurred when deleting files\n",
            proc.stderr,
        )

    def test_files_delete_multiple(self) -> None:
        proc = run_process(["files", "delete", "spam", "ham", "eggs"])
        proc.assert_failure()

        self.assertIn(
            'Failed to delete file with ID: spam. The error was: "No such File object: spam"\n'
            'Failed to delete file with ID: ham. The error was: "No such File object: ham"\n'
            'Failed to delete file with ID: eggs. The error was: "No such File object: eggs"\n'
            "One or more failures occurred when deleting files\n",
            proc.stderr,
        )
