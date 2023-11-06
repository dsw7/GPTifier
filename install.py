import sys
from argparse import ArgumentParser
from pathlib import Path
from tempfile import TemporaryDirectory
from requests import get


def download_file(url: str, file_path: Path) -> bool:
    print(f"Downloading {url} -> {file_path}")
    response = get(url, stream=True)

    if response.status_code != 200:
        print(f"URL {url} returned {response.status_code}", file=sys.stderr)
        return False

    with file_path.open("wb") as f:
        for chunk in response.iter_content(chunk_size=1024):
            if chunk:
                f.write(chunk)

    return True


def install_json(include_path: Path) -> None:
    url = "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"

    with TemporaryDirectory() as td:
        path_header = Path(td) / "json.hpp"

        if not download_file(url, path_header):
            return

        dest = include_path / "nlohmann"

        if not dest.exists():
            print(f"{dest} does not exist. Making directory")
            dest.mkdir()

        try:
            path_new = path_header.rename(dest / "json.hpp")
            print(f"{path_header} -> {path_new}")
        except PermissionError as e:
            print(f"Elevated privileges required: ({e})", file=sys.stderr)


def install_toml(include_path: Path) -> None:
    url = "https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp"

    with TemporaryDirectory() as td:
        path_header = Path(td) / "toml.hpp"

        if not download_file(url, path_header):
            return

        dest = include_path / "toml++"

        if not dest.exists():
            print(f"{dest} does not exist. Making directory")
            dest.mkdir()

        try:
            path_new = path_header.rename(dest / "toml.hpp")
            print(f"{path_header} -> {path_new}")
        except PermissionError as e:
            print(f"Elevated privileges required: ({e})", file=sys.stderr)


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
