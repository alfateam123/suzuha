import unittest
import sarge
import re

class TestSleep(unittest.TestCase):
    """
    these tests check that `settimeofday` works as described in the README.

    the related C executable code can be found at `c_tests/warp.c`
    """

    def retrieve_timings(self, output):
        start_time, future_warp, past_warp = re.findall("[0-9]+\.[0-9]+", output)
        return (float(start_time), float(future_warp), float(past_warp))

    def run_executable(self):
        preload = {"LD_PRELOAD":"../gtod.so"}
        p = sarge.run("../warp", stdout=sarge.Capture(), env=preload)
        return self.retrieve_timings(p.stdout.text)

    def test_futureWarp(self):
        """
        Test: the warp in the future (of 100 seconds) is done correctly.
        """
        start_time, future_time, _ = self.run_executable()
        self.assertTrue(future_time > start_time)
        self.assertTrue(99.9 <= future_time - start_time <= 100.1)

    def test_pastWarp_againstFutureWarp(self):
        """
        Test: the warp in the past (of 100 seconds from `future_time`) is done correctly.
        """
        _, future_time, past_time = self.run_executable()
        self.assertTrue(future_time > past_time)
        self.assertTrue(99.9 <= future_time - past_time <= 100.1)

    def test_pastWarp_isNearToStartTime(self):
        """
        Test: `past_time` must be very near to the `start_time`.
               Let's consider this: adding and then removing 100 seconds from `start_time`
               leads to `start_time`. This sentence would be true if time did not pass
               during these calculations, but time flows (:D) and we have to consider that
               `past_time` and `start_time` are near but not equal.
        """
        start_time, _, past_time  = self.run_executable()
        self.assertTrue(past_time > start_time)
        self.assertTrue(0 <= past_time - start_time <= 0.1)
