import unittest
from json import loads
from os import EX_OK
from pathlib import Path
from platform import system
from subprocess import run, PIPE, DEVNULL
from xml.etree import ElementTree
from .helpers import get_path_to_gptifier_binary

# We need to do Valgrind tests here since we're doing manual memory management in much of the libcurl code


def get_test_command(target: str, xml_file: Path) -> list[str]:
    return [
        "valgrind",
        "--xml=yes",
        f"--xml-file={xml_file}",
        get_path_to_gptifier_binary(),
        "test",
        target,
    ]


def get_leaked_bytes_from_xml(xml_file: Path) -> int:
    root = ElementTree.fromstring(xml_file.read_text())

    total_leaked_bytes = 0

    for error in root.findall("error"):
        leaked_bytes = error.find(".//leakedbytes")

        if leaked_bytes is None:
            continue

        if leaked_bytes.text is None:
            continue

        if leaked_bytes.text.isdigit():
            total_leaked_bytes += int(leaked_bytes.text)

    return total_leaked_bytes


@unittest.skipIf(system() == "Darwin", "Valgrind not supported on macOS")
class TestClientReusability(unittest.TestCase):

    def setUp(self) -> None:
        self.xml_file = Path("/tmp/results.xml")

    def tearDown(self) -> None:
        if self.xml_file.exists():
            self.xml_file.unlink()

    def test_catch_memory_leak(self) -> None:
        command = get_test_command(target="mem+", xml_file=self.xml_file)
        process = run(command, stdout=DEVNULL, stderr=PIPE)

        self.assertEqual(process.returncode, EX_OK, process.stderr.decode())
        leaked_bytes = get_leaked_bytes_from_xml(self.xml_file)
        self.assertEqual(leaked_bytes, 4, msg=f"Found {leaked_bytes} leaked bytes")

    def test_catch_no_memory_leak(self) -> None:
        command = get_test_command(target="mem-", xml_file=self.xml_file)
        process = run(command, stdout=DEVNULL, stderr=PIPE)

        self.assertEqual(process.returncode, EX_OK, process.stderr.decode())
        leaked_bytes = get_leaked_bytes_from_xml(self.xml_file)
        self.assertEqual(leaked_bytes, 0, msg=f"Found {leaked_bytes} leaked bytes")

    def test_ccc(self) -> None:
        command = get_test_command(target="ccc", xml_file=self.xml_file)
        process = run(command, stdout=PIPE, stderr=PIPE)

        self.assertEqual(process.returncode, EX_OK, process.stderr.decode())
        leaked_bytes = get_leaked_bytes_from_xml(self.xml_file)
        self.assertEqual(leaked_bytes, 0)

        stdout = loads(process.stdout.decode())
        self.assertDictEqual(
            stdout,
            {"result_1": ">>>10<<<", "result_2": ">>>10<<<", "result_3": ">>>10<<<"},
        )
