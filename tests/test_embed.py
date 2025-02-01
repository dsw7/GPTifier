from dataclasses import dataclass
from json import loads
from pathlib import Path
from unittest import TestCase
from helpers import run_process


@dataclass
class EmbedResults:
    embedding: list[float]
    model: str
    text: str


def load_embedding() -> EmbedResults:
    results = Path.home() / ".gptifier" / "embeddings.gpt"

    with results.open() as f:
        data = loads(f.read())

    return EmbedResults(
        model=data["model"], text=data["input"], embedding=data["data"][0]["embedding"]
    )


class TestEmbed(TestCase):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = run_process(["embed", option])

                proc.assert_success()
                self.assertIn("Synopsis", proc.stdout)

    def test_basic(self) -> None:
        text = "What is 3 + 5?"
        model = "text-embedding-ada-002"

        proc = run_process(["embed", f"-i{text}", f"-m{model}"])
        proc.assert_success()

        results = load_embedding()
        self.assertEqual(results.model, model)
        self.assertEqual(results.text, text)
        self.assertEqual(
            len(results.embedding), 1536
        )  # text-embedding-ada-002 dimension

    def test_read_from_file(self) -> None:
        input_text_file = Path(__file__).resolve().parent / "prompt_basic.txt"
        model = "text-embedding-3-small"

        proc = run_process(["embed", f"-r{input_text_file}", f"-m{model}"])
        proc.assert_success()

        results = load_embedding()
        self.assertEqual(results.model, model)
        self.assertEqual(results.text, input_text_file.read_text())

    def test_missing_input_file(self) -> None:
        proc = run_process(["embed", "--read-from-file=/tmp/yU8nnkRs.txt"])
        proc.assert_failure()
        self.assertIn("Could not open file '/tmp/yU8nnkRs.txt'", proc.stderr)

    def test_invalid_model(self) -> None:
        proc = run_process(["embed", "-i'What is 3 + 5?'", "-mfoobar"])
        proc.assert_failure()
        self.assertIn(
            "The model `foobar` does not exist or you do not have access to it.",
            proc.stderr,
        )


class TestEmbedReadFromInputfile(TestCase):
    def setUp(self) -> None:
        self.filename = Path.cwd() / "Inputfile"
        self.filename.write_text("A foo that bars!")

    def tearDown(self) -> None:
        self.filename.unlink()

    def test_read_from_inputfile(self) -> None:
        proc = run_process("embed")
        proc.assert_success()
        self.assertIn("Found an Inputfile in current working directory!", proc.stdout)
