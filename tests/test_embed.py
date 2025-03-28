from dataclasses import dataclass
from json import loads
from pathlib import Path
from tempfile import gettempdir
from .extended_testcase import TestCaseExtended


@dataclass
class Embedding:
    embedding: list[float]
    model: str
    text: str


def load_embedding(results_file: Path) -> Embedding:
    with results_file.open() as f:
        data = loads(f.read())

    return Embedding(
        model=data["model"], text=data["input"], embedding=data["embedding"]
    )


def get_cosine_similarity(left: Embedding, right: Embedding) -> float:
    dot_p: float = 0

    for l, r in zip(left.embedding, right.embedding):
        dot_p += l * r

    mag_l: float = (sum(i**2 for i in left.embedding)) ** 0.5
    mag_r: float = (sum(i**2 for i in right.embedding)) ** 0.5

    return dot_p / (mag_l * mag_r)


class TestEmbed(TestCaseExtended):

    def test_help(self) -> None:
        for option in ["-h", "--help"]:
            with self.subTest(option=option):
                proc = self.assertSuccess("embed", option)
                self.assertIn(
                    "Get embedding representing a block of text.", proc.stdout
                )

    def test_missing_input_file(self) -> None:
        proc = self.assertFailure("embed", "--read-from-file=/tmp/yU8nnkRs.txt")
        self.assertIn("Could not open file '/tmp/yU8nnkRs.txt'", proc.stderr)

    def test_invalid_model(self) -> None:
        proc = self.assertFailure("embed", "-i'What is 3 + 5?'", "-mfoobar")
        self.assertIn(
            "The model `foobar` does not exist or you do not have access to it.",
            proc.stderr,
        )


class TestEmbedCosineSimilarityIdentical(TestCaseExtended):

    def setUp(self) -> None:
        self.input_file = Path(__file__).resolve().parent / "prompt_basic.txt"
        self.output_file = Path(gettempdir()) / "result.gpt"

    def tearDown(self) -> None:
        if self.output_file.exists():
            self.output_file.unlink()

    def test_compute_cosine_similarities(self) -> None:
        model = "text-embedding-3-small"
        self.assertSuccess(
            "embed", f"-r{self.input_file}", f"-m{model}", f"-o{self.output_file}"
        )

        embedding = load_embedding(self.output_file)

        self.assertEqual(model, embedding.model)
        self.assertEqual(embedding.text, self.input_file.read_text())

        self.assertEqual(
            len(embedding.embedding), 1536
        )  # text-embedding-ada-002 dimension
        self.assertAlmostEqual(
            get_cosine_similarity(embedding, embedding), 1.00, places=2
        )


class TestEmbedCosineSimilarityOrthogonal(TestCaseExtended):

    def setUp(self) -> None:
        self.filename_1 = Path(gettempdir()) / "result_1.gpt"
        self.filename_2 = Path(gettempdir()) / "result_2.gpt"

    def tearDown(self) -> None:
        if self.filename_1.exists():
            self.filename_1.unlink()

        if self.filename_2.exists():
            self.filename_2.unlink()

    def test_compute_cosine_similarities(self) -> None:
        model = "text-embedding-ada-002"

        text_1 = "The cat meowed softly."
        self.assertSuccess("embed", f"-i{text_1}", f"-m{model}", f"-o{self.filename_1}")
        embedding_1 = load_embedding(self.filename_1)

        text_2 = "Quantum physics is fascinating."
        self.assertSuccess("embed", f"-i{text_2}", f"-m{model}", f"-o{self.filename_2}")
        embedding_2 = load_embedding(self.filename_2)

        self.assertTrue(0.6 <= get_cosine_similarity(embedding_1, embedding_2) <= 0.8)
