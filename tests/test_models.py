from .extended_testcase import TestCaseExtended


class TestModels(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("models", option)
                self.assertIn("List available OpenAI or user models", proc.stdout)

    pattern = r"Creation time\s+Owner\s+Model ID"

    def test_models_openai(self) -> None:
        proc = self.assertSuccess("models")
        self.assertRegex(proc.stdout, self.pattern)

    def test_models_user(self) -> None:
        for option in ["-u", "--user"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("models", option)
                self.assertRegex(proc.stdout, self.pattern)

    def test_models_raw_json(self) -> None:
        for option in ["-j", "--json"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("models", option)
                proc.load_stdout_to_json()
