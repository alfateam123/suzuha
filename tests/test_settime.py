import unittest
import sarge
import re


class TestSetTime(unittest.TestCase):
    """
    In these tests, calls to shim'd `settimeofday`
    are checked.
    """

    def retrieve_timings(self, output):
        # we expect only (no more, not less) two floats in the output.
        # it'll break if more. it's a test, it's good :D
        start_time, end_time = re.findall("[0-9]+\.[0-9]+", output)
        return (float(start_time), float(end_time))

    def run_executable(self, with_shim=False):
        preload = (with_shim and {"LD_PRELOAD": "../gtod.so"} or dict())
        p = sarge.run("../settime", stdout=sarge.Capture(), env=preload)
        return self.retrieve_timings(p.stdout.text)

    def test_withoutShim(self):
        start_time, end_time = self.run_executable()
        self.assertGreater(end_time, start_time)

    def test_withShim(self):
        start_time, end_time = self.run_executable(with_shim=True)
        self.assertLess(end_time, start_time)
