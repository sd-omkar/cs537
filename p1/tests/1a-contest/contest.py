import hashlib
from time import sleep
import pickle

import toolspath
from testing.test import *

from check import check_sorted
from gentests import GenTest

class Canary(GenTest):
   name = "canary"
   description = "100k records"
   timeout = 15
   seed = 0
   n = 100000

# tests that use the generate utility and then check the output

def median(vals):
   if len(vals) % 2 == 1:
      return vals[(len(vals) + 1)/ 2 - 1]
   else:
      a = vals[len(vals)/2-1]
      b = vals[len(vals)/2]
      return (a + b) / 2

class Contest(Test):
   name = "contest"
   description = "contest"
   timeout = 1000
   seeds = [0, 1, 2]
   n = 100000
   runs = 10

   def run(self):
      generate_path = self.test_path + "/generate"
      infile = self.project_path + "/infile"
      outfile = self.project_path + "/outfile"

      times = list()

      for seed in self.seeds:
         sorted_digest = None

         self.log("Generating input file")
         status = self.run_util([generate_path, "-s", str(seed),
            "-n", str(self.n), "-o", infile])
         if status != 0:
            raise Exception("generate failed with error " + str(status))

         for i in range(self.runs):

            sleep(0.5)
            self.log("Running fastsort")
            child = self.runexe(["fastsort", "-i", infile, "-o", outfile],
                  status = 0, stderr = "")
            if status != 0:
               raise Failure("fastsort exited with status " + str(status))
            self.log("Fastsort completed in " + str(child.wallclock_time) +
                  " seconds")
            times.append(child.wallclock_time);

            self.log("Checking output")
            m = hashlib.md5()
            with open(outfile, "rb") as f:
               m.update(f.read())
            if sorted_digest is None or m.digest() != sorted_digest:
               (is_sorted, msg) = check_sorted(outfile, infile)
               if not is_sorted:
                  self.fail(msg)
                  raise Failure(msg)
               sorted_digest = m.digest()
            os.remove(outfile)

            #self.run_util(["sync"])

      with open("times.pickle", "wb") as f:
         pickle.dump(times, f)

      times = sorted(times)

      #print times

      print "best  ", times[0]
      print "median", median(times)
      print "worst ", times[-1]



      self.done()


test_list = [Canary, Contest]
