from .extended_testcase import TestCaseExtended


class TestShort(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("short", option)
                self.assertIn(
                    "Create a chat completion but without threading or verbosity.",
                    proc.stdout,
                )

    prompt = '"What is 2 + 2? Format the result as follows: >>>{result}<<<"'

    def test_short_prompt(self) -> None:
        proc = self.assertSuccess("short", "--temperature=1.00", self.prompt)
        self.assertIn(">>>4<<<", proc.stdout)

    def test_short_raw_json(self) -> None:
        for option in ["-j", "--json"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("short", option, self.prompt)
                results = proc.load_stdout_to_json()
                self.assertIn(">>>4<<<", results["choices"][0]["message"]["content"])

    def test_invalid_temp_stof(self) -> None:
        proc = self.assertFailure("short", "-tfoobar", self.prompt)
        self.assertIn("Failed to convert 'foobar' to float", proc.stderr)

    def test_invalid_temp_low(self) -> None:
        proc = self.assertFailure("short", "-t-2.5", self.prompt)
        self.assertIn("Temperature must be between 0 and 2", proc.stderr)

    def test_invalid_temp_high(self) -> None:
        proc = self.assertFailure("short", "-t2.5", self.prompt)
        self.assertIn("Temperature must be between 0 and 2", proc.stderr)

    def test_missing_prompt(self) -> None:
        proc = self.assertFailure("short")
        self.assertIn("Prompt is empty", proc.stderr)

    def test_empty_prompt(self) -> None:
        proc = self.assertFailure("short", "")
        self.assertIn("Prompt is empty", proc.stderr)
