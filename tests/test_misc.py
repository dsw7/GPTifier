from .extended_testcase import TestCaseExtended


class TestUnknownCommand(TestCaseExtended):

    def test_unknown_command(self) -> None:
        proc = self.assertFailure("foobar")
        self.assertEqual(
            proc.stderr.strip(), "Received unknown command. Re-run with -h or --help"
        )


class TestCatchMemoryLeak(TestCaseExtended):

    def test_catch_memory_leak(self) -> None:
        proc = self.assertSuccess("test", "leak")
        self.assertEqual(proc.stdout.strip(), "5")


class TestCurlHandleReusability(TestCaseExtended):

    def test_reuse_curl_handle(self) -> None:
        # Test that curl handle can be reused in GPTifier's network layer

        proc = self.assertSuccess("test", "ccc")
        self.assertDictEqual(
            proc.load_stdout_to_json(),
            {"result_1": ">>>10<<<", "result_2": ">>>10<<<", "result_3": ">>>10<<<"},
        )
