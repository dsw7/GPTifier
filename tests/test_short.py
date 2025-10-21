import typing
from .extended_testcase import TestCaseExtended


class TestShort(TestCaseExtended):

    def load_output(self, output: typing.Any) -> str:
        self.assertTrue(len(output) > 0, "Output is empty")
        text: str | None = None

        for item in output:
            if item["type"] != "message":
                continue

            if item["status"] != "completed":
                continue

            if item["content"][0]["type"] == "output_text":
                text = item["content"][0]["text"]
                break

        self.assertIsNotNone(text, "Could not find text in OpenAI output")
        # Can ignore this since assertion will break out of function if text is null
        return text  # type: ignore

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("short", option)
                self.assertIn(
                    "Create a response but without threading or verbosity.", proc.stdout
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
                self.assertIn(">>>4<<<", self.load_output(results["output"]))

    def test_empty_temp(self) -> None:
        proc = self.assertFailure("short", "--temperature=", self.prompt)
        self.assertIn("Empty temperature", proc.stderr)

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
