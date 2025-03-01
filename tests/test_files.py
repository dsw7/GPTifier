from pathlib import Path
from unittest import TestCase
from .helpers import run_process


class TestFiles(TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["files", option])
                proc.assert_success()
                self.assertIn("Manage files uploaded to OpenAI.", proc.stdout)

    def test_upload_then_delete(self) -> None:
        jsonl_file = Path(__file__).resolve().parent / "dummy.jsonl"

        proc = run_process(["fine-tune", "upload-file", str(jsonl_file)])
        proc.assert_success()
        last_line = proc.stdout.strip().rsplit("\n", maxsplit=1)[-1]
        file_id = last_line.split(": ")[-1]

        proc = run_process(["files", "delete", file_id])
        proc.assert_success()


class TestFilesList(TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["files", "list", option])
                proc.assert_success()
                self.assertIn("List uploaded files.", proc.stdout)

    pattern = r"File ID\s+Filename\s+Creation time\s+Purpose"

    def test_files_list_default(self) -> None:
        proc = run_process("files")
        proc.assert_success()
        self.assertRegex(proc.stdout, self.pattern)

    def test_files_list(self) -> None:
        proc = run_process(["files", "list"])
        proc.assert_success()
        self.assertRegex(proc.stdout, self.pattern)

    def test_files_list_raw_json(self) -> None:
        for option in ["-j", "--json"]:
            with self.subTest(option=option):
                proc = run_process(["files", "list", option])
                proc.assert_success()
                proc.load_stdout_to_json()


class TestFilesDelete(TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["files", "delete", option])
                proc.assert_success()
                self.assertIn("Delete an uploaded file.", proc.stdout)

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
