from time import time

import toolspath
from testing.test import *

from check import check_sorted

class DupTest(Test):
   name = "dup"
   description = "duplicate records"
   timeout = 15

   def run(self):
      infile = self.test_path + "/dupdata"
      outfile = self.project_path + "/outfile"

      self.log("Running fastsort")
      child = self.runexe(["fastsort", "-i", infile, "-o", outfile],
          status = 0, stderr = "")
      self.log("Fastsort completed in " + str(child.wallclock_time) + " seconds")
      self.log("Checking output")
      (is_sorted, msg) = check_sorted(outfile, infile)
      if not is_sorted:
         raise Failure(msg)
      self.done()

test_list = [DupTest]
