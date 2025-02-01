from pathlib import Path
from unittest import TestCase
from helpers import run_process


class TestFineTune(TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["fine-tune", option])
                proc.assert_success()
                self.assertIn("Synopsis", proc.stdout)

    def test_subcommand_help(self) -> None:
        for subcommand in ["upload-file", "create-job", "delete-model", "list-jobs"]:
            for option in ["-h", "--help"]:
                with self.subTest(subcommand=subcommand, option=option):
                    proc = run_process(["fine-tune", subcommand, option])
                    proc.assert_success()
                    self.assertIn("Synopsis", proc.stdout)

    def test_upload_missing_file(self) -> None:
        proc = run_process(["fine-tune", "upload-file", "foobar"])
        proc.assert_failure()
        self.assertIn(
            "Failed to open/read local data from file/application", proc.stderr
        )

    def test_upload_invalid_file(self) -> None:
        input_text_file = Path(__file__).resolve().parent / "prompt_basic.txt"
        proc = run_process(["fine-tune", "upload-file", str(input_text_file)])
        proc.assert_failure()
        self.assertIn(
            "Invalid file format for Fine-Tuning API. Must be .jsonl", proc.stderr
        )

    def test_create_job_invalid_params(self) -> None:
        process = run_process(
            ["fine-tune", "create-job", "--model=foobar", "--file-id=foobar"]
        )
        process.assert_failure()
        self.assertIn("invalid training_file: foobar", process.stderr)

    def test_delete_model_missing_model(self) -> None:
        process = run_process(["fine-tune", "delete-model", "foobar"])
        process.assert_failure()
        self.assertIn("The model 'foobar' does not exist", process.stderr)

    def test_list_jobs_raw(self) -> None:
        for option in ["-r", "--raw"]:
            with self.subTest(option=option):
                proc = run_process(["fine-tune", "list-jobs", option])
                proc.assert_success()
                proc.load_stdout_to_json()

    def test_list_jobs(self) -> None:
        for option in ["-l1", "--limit=1"]:
            with self.subTest(option=option):
                proc = run_process(["fine-tune", "list-jobs", option])
                proc.assert_success()
                self.assertNotIn("No limit passed with --limit flag.", proc.stdout)
