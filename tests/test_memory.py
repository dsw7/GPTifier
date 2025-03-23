from os import getenv, EX_OK
from pathlib import Path
from subprocess import run, PIPE, DEVNULL
from unittest import TestCase
from xml.etree import ElementTree


def get_test_command(target: str, xml_file: Path) -> list[str]:
    path_bin: str | None = getenv("PATH_BIN")

    if path_bin is None:
        raise SystemExit("Could not locate PATH_BIN environment variable")

    return ["valgrind", "--xml=yes", f"--xml-file={xml_file}", path_bin, "test", target]


def get_leaked_bytes_from_xml(xml_file: Path) -> int | None:
    root = ElementTree.fromstring(xml_file.read_text())

    leaked_bytes: ElementTree.Element | None = root.find(".//leakedbytes")

    if leaked_bytes is None:
        return None

    if leaked_bytes.text is None:
        return None

    return int(leaked_bytes.text)


class TestMemory(TestCase):

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
        self.assertEqual(leaked_bytes, 4)

    def test_catch_no_memory_leak(self) -> None:
        command = get_test_command(target="mem-", xml_file=self.xml_file)
        process = run(command, stdout=DEVNULL, stderr=PIPE)

        self.assertEqual(process.returncode, EX_OK, process.stderr.decode())
        leaked_bytes = get_leaked_bytes_from_xml(self.xml_file)
        self.assertIsNone(leaked_bytes, msg=f"Found {leaked_bytes} leaked bytes")
