import sys
from argparse import ArgumentParser
from pathlib import Path
from tempfile import gettempdir, TemporaryDirectory
from requests import get


def download_file(url: str, file_path: Path) -> None:
    response = get(url, stream=True)

    with open(file_path, "wb") as f:
        for chunk in response.iter_content(chunk_size=1024):
            if chunk:
                f.write(chunk)


def main() -> None:
    parser = ArgumentParser(description="Install GPTifier compilation dependencies")
    parser.add_argument("include_path", help="Where to install headers")

    args = parser.parse_args()
    include_path = Path(args.include_path)

    if not include_path.exists():
        sys.exit(f"Path {include_path} does not exist")

    with TemporaryDirectory() as td:
        print(td)


if __name__ == "__main__":
    main()
