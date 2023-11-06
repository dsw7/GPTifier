from argparse import ArgumentParser
from pathlib import Path


def main() -> None:
    parser = ArgumentParser(description="Install GPTifier compilation dependencies")
    parser.add_argument("include_path", help="Where to install headers")

    args = parser.parse_args()
    include_path = Path(args.include_path)
    print(include_path)


if __name__ == "__main__":
    main()
