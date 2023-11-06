import sys
from argparse import ArgumentParser
from pathlib import Path
from tempfile import gettempdir, TemporaryDirectory
from requests import get


def download_file(url: str, file_path: Path) -> None:
    print(f"Downloading: {url}")
    print(f"Downloading to: {file_path}")

    response = get(url, stream=True)

    with file_path.open("wb") as f:
        for chunk in response.iter_content(chunk_size=1024):
            if chunk:
                f.write(chunk)


def install_json(include_path: Path) -> None:
    url = "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
    dest = include_path / "nlohmann"

    with TemporaryDirectory() as td:
        path_header = Path(td) / "json.hpp"
        download_file(url, path_header)

        if not dest.exists():
            print(f"{dest} does not exist. Making directory")
            dest.mkdir()


def install_toml(include_path: Path) -> None:
    url = "https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp"
    dest = include_path / "toml++"

    with TemporaryDirectory() as td:
        path_header = Path(td) / "toml.hpp"
        download_file(url, path_header)

        if not dest.exists():
            print(f"{dest} does not exist. Making directory")
            dest.mkdir()


def main() -> None:
    parser = ArgumentParser(description="Install GPTifier compilation dependencies")
    parser.add_argument("include_path", help="Where to install headers")

    args = parser.parse_args()
    include_path = Path(args.include_path)

    if not include_path.exists():
        sys.exit(f"Path {include_path} does not exist")

    install_json(include_path)
    install_toml(include_path)


if __name__ == "__main__":
    main()
