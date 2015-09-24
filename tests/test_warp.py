import unittest
import sarge
import re

class TestSleep(unittest.TestCase):
    """
    these tests check that `settimeofday` works as described in the README.
    """

    def retrieve_timings(self, output):
        start_time, future_warp, past_warp = re.findall("[0-9]+\.[0-9]+", output)
        return (float(start_time), float(future_warp), float(past_warp))

    def run_executable(self):
        preload = {"LD_PRELOAD":"../gtod.so"}
        p = sarge.run("../warp", stdout=sarge.Capture(), env=preload)
        return self.retrieve_timings(p.stdout.text)

    def test_futureWarp(self):
        start_time, future_time, _ = self.run_executable()
        self.assertTrue(future_time > start_time)
        self.assertTrue(99.9 <= future_time - start_time <= 100.1)

    def test_pastWarp_againstFutureWarp(self):
        _, future_time, past_time = self.run_executable()
        self.assertTrue(future_time > past_time)
        self.assertTrue(99.9 <= future_time - past_time <= 100.1)

    def test_pastWarp_isNearToStartTime(self):
        start_time, _, past_time  = self.run_executable()
        self.assertTrue(past_time > start_time)
        self.assertTrue(-0.1 <= past_time - start_time <= 0.1) 
