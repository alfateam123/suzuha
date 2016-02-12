import unittest
import sarge
import re

class TestTimeSleep(unittest.TestCase):
    """
    these tests are here to check whenever
    the applications can run ok both with
    and without the shim library.

    You cannot know _from the inside_ whenever you're
    using the shim or not, we have to parse the specially crafted
    output from the tests to understand if they're ok or not.
    """

    def retrieve_timings(self, output):
        #we expect only (no more, not less) two floats in the output.
        #it'll break if more. it's a test, it's good :D
        (start_time, end_time) = map(str.strip, re.findall("[0-9]+\n", output))
        return (int(start_time), int(end_time))

    def run_executable(self, with_shim=False):
        preload = (with_shim and {"LD_PRELOAD":"../gtod.so"} or dict())
        p = sarge.run("../time", stdout=sarge.Capture(), env=preload)
        return self.retrieve_timings(p.stdout.text)

    def test_withoutShim(self):
        start_time, end_time  = self.run_executable()
        self.assertTrue(1 <= (end_time - start_time) <= 2)

    def test_withShim(self):
        start_time, end_time = self.run_executable(with_shim=True)
        self.assertTrue(99 <= abs(end_time - start_time) <= 102) 
