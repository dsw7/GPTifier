from dataclasses import dataclass
from json import loads
from pathlib import Path
from tempfile import NamedTemporaryFile, gettempdir
from time import time
from .extended_testcase import TestCaseExtended


@dataclass
class Response:
    created: int
    input_: str
    input_tokens: int
    model: str
    output: str
    output_tokens: int
    rtt: float


def load_content(json_file: str) -> Response:
    with open(json_file) as f:
        contents = loads(f.read())

    return Response(
        created=contents["created"],
        input_=contents["input"],
        input_tokens=contents["input_tokens"],
        model=contents["model"],
        output=contents["output"],
        output_tokens=contents["output_tokens"],
        rtt=contents["rtt"],
    )


class TestResponseReadFromInputfile(TestCaseExtended):
    def setUp(self) -> None:
        self.filename = Path.cwd() / "Inputfile"

    def tearDown(self) -> None:
        self.filename.unlink()

    def test_read_from_inputfile(self) -> None:
        prompt = "What is 1 + 3? Format the result as follows: >>>{result}<<<"
        completion = ">>>4<<<"
        self.filename.write_text(prompt)

        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            self.assertSuccess("run", "-t0", f"-o{json_file}")
            content = load_content(json_file)
            self.assertEqual(content.output, completion)


class TestResponseJSON(TestCaseExtended):
    prompt = "What is 1 + 4? Format the result as follows: >>>{result}<<<"
    completion = ">>>5<<<"

    def setUp(self) -> None:
        with NamedTemporaryFile(dir=gettempdir()) as f:
            t_start = time()
            # Yes... a hack :)
            self.assertSuccess("run", f"-p{self.prompt}", "-t0", f"-o{f.name}")
            self.rtt = time() - t_start
            self.content = load_content(f.name)

    def test_prompt_matches_input(self) -> None:
        self.assertEqual(self.content.input_, self.prompt)

    def test_output_matches_completion(self) -> None:
        self.assertEqual(self.content.output, self.completion)

    def test_correct_input_tokens_count(self) -> None:
        self.assertEqual(self.content.input_tokens, 26)

    def test_correct_output_tokens_count(self) -> None:
        self.assertEqual(self.content.output_tokens, 4)

    def test_approx_rtt(self) -> None:
        diff = abs(self.content.rtt - self.rtt)
        self.assertLessEqual(diff, 0.25, "RTT times are not within 0.25 seconds")

    def test_approx_created(self) -> None:
        diff = abs(self.content.created - int(time()))
        self.assertLessEqual(diff, 2.0, "Creation times are not within 2 seconds")


class TestResponse(TestCaseExtended):
    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("run", option)
                self.assertIn("Create a response according to a prompt.", proc.stdout)

    def test_read_from_file(self) -> None:
        prompt = Path(__file__).resolve().parent / "test_run" / "prompt_basic.txt"

        with NamedTemporaryFile(dir=gettempdir()) as f:
            json_file = f.name
            self.assertSuccess("run", f"-r{prompt}", "-t0", f"-o{json_file}")
            content = load_content(json_file)
            self.assertEqual(content.output, ">>>8<<<")

    prompt = "What is 1 + 1? Format the result as follows: >>>{result}<<<"

    def test_write_to_stdout(self) -> None:
        proc = self.assertSuccess("run", f"-p'{self.prompt}'")
        self.assertIn(">>>2<<<", proc.stdout)

    def test_invalid_temp(self) -> None:
        proc = self.assertFailure("run", f"-p'{self.prompt}'", "-tfoobar")
        self.assertIn("Failed to convert 'foobar' to float", proc.stderr)

    def test_empty_temp(self) -> None:
        proc = self.assertFailure("run", "-p'A foo that bars?'", "--temperature=")
        self.assertIn("Empty temperature", proc.stderr)

    def test_missing_prompt_file(self) -> None:
        proc = self.assertFailure("run", "--read-from-file=/tmp/yU8nnkRs.txt")
        self.assertIn("Unable to open '/tmp/yU8nnkRs.txt'", proc.stderr)

    def test_empty_prompt(self) -> None:
        proc = self.assertFailure("run", "--prompt=")
        self.assertIn("No input text provided anywhere. Cannot proceed", proc.stderr)

    def test_empty_prompt_file(self) -> None:
        proc = self.assertFailure("run", "--read-from-file=")
        self.assertIn("Empty prompt filename", proc.stderr)

    def test_invalid_output_file_location(self) -> None:
        proc = self.assertFailure(
            "run", f"--prompt='{self.prompt}'", "--file=/tmp/a/b/c"
        )
        self.assertIn("Unable to open '/tmp/a/b/c'", proc.stderr)

    def test_empty_output_file(self) -> None:
        proc = self.assertFailure("run", "--prompt='What is 7 + 2?'", "--file=")
        self.assertIn("No filename provided", proc.stderr)

    def test_empty_model(self) -> None:
        proc = self.assertFailure("run", "-p'foobar'", "--model=")
        self.assertIn("Model is empty", proc.stderr)

    def test_out_of_range_temp(self) -> None:
        for temp in [-2.5, 2.5]:
            with self.subTest(temp=temp):
                proc = self.assertFailure("run", f"-p'{self.prompt}'", f"-t{temp}")
                self.assertIn("Temperature must be between 0 and 2", proc.stderr)


class TestCompatibleModels(TestCaseExtended):
    def test_misc_valid_models(self) -> None:
        prompt = "What is 1 + 1?"
        for model in [
            "codex-mini-latest",
            "gpt-3.5-turbo",
            "gpt-4",
            "gpt-4.1",
            "gpt-4.1-mini",
            "gpt-4.1-nano",
            "gpt-4o",
            "gpt-4o-mini",
            "o1",
            "o1-pro",
            "o3-mini",
            "o4-mini",
        ]:
            with self.subTest(model=model):
                self.assertSuccess("run", f"-p'{prompt}'", f"-m{model}")


class TestIncompatibleModels(TestCaseExtended):
    prompt = "What is 1 + 1?"

    def test_non_existent_model(self) -> None:
        proc = self.assertFailure("run", f"-p'{self.prompt}'", "-mfoobar")
        self.assertIn(
            "The requested model 'foobar' does not exist.\nCannot proceed", proc.stderr
        )

    def test_wrong_endpoint_model_not_supported(self) -> None:
        for model in [
            "gpt-4o-transcribe",
            "gpt-image-1",
            "o1-mini",
            "tts-1",
            "tts-1-hd",
            "whisper-1",
        ]:
            with self.subTest(model=model):
                proc = self.assertFailure("run", f"-p'{self.prompt}'", f"-m{model}")
                self.assertIn(
                    f"The requested model '{model}' is not supported with the Responses API.\nCannot proceed\n",
                    proc.stderr,
                )

    def test_wrong_endpoint_model_not_found(self) -> None:
        for model in [
            "dall-e-2",
            "dall-e-3",
            "davinci-002",
            "text-embedding-3-large",
            "text-embedding-3-small",
        ]:
            with self.subTest(model=model):
                proc = self.assertFailure("run", f"-p'{self.prompt}'", f"-m{model}")
                self.assertIn(
                    f"The requested model, '{model}' was not found.\nCannot proceed\n",
                    proc.stderr,
                )

    def test_unverified_organization_error(self) -> None:
        for model in ["gpt-5", "gpt-5-mini", "gpt-5-codex", "o3"]:
            with self.subTest(model=model):
                proc = self.assertFailure("run", f"-p'{self.prompt}'", f"-m{model}")
                self.assertIn(
                    f"Your organization must be verified to use the model '{model}'.",
                    proc.stderr,
                )

    def test_wrong_endpoint_deep_research(self) -> None:
        model = "o4-mini-deep-research"
        proc = self.assertFailure("run", f"-p'{self.prompt}'", f"-m{model}")
        self.assertIn(
            "Deep research models require at least one of 'web_search_preview', 'mcp', or 'file_search' tools.\nCannot proceed\n",
            proc.stderr,
        )

    def test_sora_2(self) -> None:
        for model in ["sora-2", "sora-2-pro"]:
            with self.subTest(model=model):
                proc = self.assertFailure("run", f"-p'{self.prompt}'", f"-m{model}")
                self.assertIn(f"Model not found {model}", proc.stderr)
