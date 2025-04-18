from pathlib import Path
from .extended_testcase import TestCaseExtended


class TestFiles(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("files", option)
                self.assertIn("Manage files uploaded to OpenAI.", proc.stdout)

    def test_upload_then_delete(self) -> None:
        jsonl_file = Path(__file__).resolve().parent / "test_files" / "dummy.jsonl"

        proc = self.assertSuccess("fine-tune", "upload-file", str(jsonl_file))
        last_line = proc.stdout.strip().rsplit("\n", maxsplit=1)[-1]
        file_id = last_line.split(": ")[-1]

        proc = self.assertSuccess("files", "delete", file_id)


class TestFilesList(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("files", "list", option)
                self.assertIn("List uploaded files.", proc.stdout)

    pattern = r"File ID\s+Filename\s+Creation time\s+Purpose"

    def test_files_list_default(self) -> None:
        proc = self.assertSuccess("files")
        self.assertRegex(proc.stdout, self.pattern)

    def test_files_list(self) -> None:
        proc = self.assertSuccess("files", "list")
        self.assertRegex(proc.stdout, self.pattern)

    def test_files_list_raw_json(self) -> None:
        for option in ["-j", "--json"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("files", "list", option)
                proc.load_stdout_to_json()


class TestFilesDelete(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("files", "delete", option)
                self.assertIn("Delete an uploaded file.", proc.stdout)

    def test_files_delete(self) -> None:
        proc = self.assertFailure("files", "delete", "foobar")
        self.assertIn(
            'Failed to delete file with ID: foobar. The error was: "No such File object: foobar"\n'
            "One or more failures occurred when deleting files\n",
            proc.stderr,
        )

    def test_files_delete_multiple(self) -> None:
        proc = self.assertFailure("files", "delete", "spam", "ham", "eggs")
        self.assertIn(
            'Failed to delete file with ID: spam. The error was: "No such File object: spam"\n'
            'Failed to delete file with ID: ham. The error was: "No such File object: ham"\n'
            'Failed to delete file with ID: eggs. The error was: "No such File object: eggs"\n'
            "One or more failures occurred when deleting files\n",
            proc.stderr,
        )
