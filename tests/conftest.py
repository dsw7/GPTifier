import typing
import pytest
import requests

_OLLAMA_HOST = "localhost"
_OLLAMA_PORT = 11434


def _is_ollama_server_dead() -> bool:
    try:
        requests.get(f"http://{_OLLAMA_HOST}:{_OLLAMA_PORT}", timeout=5)
    except requests.exceptions.ConnectionError:
        return True

    return False


def pytest_collection_modifyitems(items: list[typing.Any]) -> None:
    # function intercepts list of tests after collecting tests but
    # before actually running any tests

    if _is_ollama_server_dead():
        skip_marker = pytest.mark.skip(reason="Cannot connect to Ollama server")

        for item in items:
            if "test_ollama" in item.keywords:
                item.add_marker(skip_marker)
