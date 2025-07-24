from pathlib import Path
from .extended_testcase import TestCaseExtended


class TestFineTune(TestCaseExtended):
    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("fine-tune", option)
                self.assertIn("Manage all fine-tuning operations", proc.stdout)


class TestFineTuneUploadFile(TestCaseExtended):
    def test_no_args_or_opts(self) -> None:
        proc = self.assertFailure("fine-tune", "upload-file")
        self.assertIn("A fine tuning file needs to be provided", proc.stderr)

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            proc = self.assertSuccess("fine-tune", "upload-file", option)
            self.assertIn("Upload a fine-tuning file", proc.stdout)

    def test_upload_missing_file(self) -> None:
        proc = self.assertFailure("fine-tune", "upload-file", "foobar")
        self.assertIn(
            "Failed to open/read local data from file/application", proc.stderr
        )

    def test_upload_empty_filename(self) -> None:
        proc = self.assertFailure("fine-tune", "upload-file", "")
        self.assertIn("Filename is empty", proc.stderr)

    def test_upload_invalid_file(self) -> None:
        input_text_file = Path(__file__).resolve().parent / "__init__.py"
        proc = self.assertFailure("fine-tune", "upload-file", str(input_text_file))
        self.assertIn(
            "Invalid file format for Fine-Tuning API. Must be .jsonl", proc.stderr
        )


class TestFineTuneCreateJob(TestCaseExtended):
    def test_no_args_or_opts(self) -> None:
        proc = self.assertFailure("fine-tune", "create-job")
        self.assertIn(
            "A fine tuning file ID and model needs to be provided", proc.stderr
        )

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            proc = self.assertSuccess("fine-tune", "create-job", option)
            self.assertIn("Create a fine-tuning job", proc.stdout)

    def test_create_job_invalid_option(self) -> None:
        proc = self.assertFailure("fine-tune", "create-job", "foobar")
        self.assertIn("Unknown option: 'foobar'", proc.stderr)

    def test_create_job_invalid_training_file(self) -> None:
        proc = self.assertFailure("fine-tune", "create-job", "foobar", "gpt-3.5-turbo")
        self.assertIn("invalid training_file: foobar", proc.stderr)

    def test_create_job_empty_training_file(self) -> None:
        proc = self.assertFailure("fine-tune", "create-job", "", "gpt-3.5-turbo")
        self.assertIn("Training file ID argument is empty", proc.stderr)

    def test_create_job_empty_model(self) -> None:
        proc = self.assertFailure("fine-tune", "create-job", "foobar", "")
        self.assertIn("Model argument is empty", proc.stderr)


class TestFineTuneDeleteModel(TestCaseExtended):
    def test_no_args_or_opts(self) -> None:
        proc = self.assertFailure("fine-tune", "delete-model")
        self.assertIn("A model ID needs to be provided", proc.stderr)

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            proc = self.assertSuccess("fine-tune", "delete-model", option)
            self.assertIn("Delete a fine-tuned model", proc.stdout)

    def test_delete_model_missing_model(self) -> None:
        proc = self.assertFailure("fine-tune", "delete-model", "foobar")
        self.assertIn("The model 'foobar' does not exist", proc.stderr)

    def test_delete_model_empty_model_id_arg(self) -> None:
        proc = self.assertFailure("fine-tune", "delete-model", "")
        self.assertIn("Model ID argument is empty", proc.stderr)


class TestFineTuneListJobs(TestCaseExtended):
    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            proc = self.assertSuccess("fine-tune", "list-jobs", option)
            self.assertIn("List fine-tuning jobs", proc.stdout)

    def test_list_jobs_raw_json(self) -> None:
        for option in ["-j", "--json"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("fine-tune", "list-jobs", option)
                proc.load_stdout_to_json()

    def test_list_jobs(self) -> None:
        for option in ["-l1", "--limit=1"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("fine-tune", "list-jobs", option)
                self.assertNotIn("No limit passed with --limit flag.", proc.stdout)

    def test_list_jobs_stoi(self) -> None:
        proc = self.assertFailure("fine-tune", "list-jobs", "--limit=abc")
        self.assertIn("Failed to convert 'abc' to int", proc.stderr)

    def test_list_jobs_invalid_limit(self) -> None:
        proc = self.assertFailure("fine-tune", "list-jobs", "--limit=-5")
        self.assertIn("Limit must be greater than or equal to 1", proc.stderr)
