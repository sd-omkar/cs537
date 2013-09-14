import toolspath
from testing.test import *

from check import check_sorted

# tests that use the generate utility and then check the output

class GenTest(Test):
   seed = 0
   n = 0

   def run(self):
      generate_path = self.test_path + "/generate"
      infile = self.project_path + "/infile"
      outfile = self.project_path + "/outfile"

      self.log("Generating input file")
      status = self.run_util([generate_path, "-s", str(self.seed),
         "-n", str(self.n), "-o", infile])
      if status != 0:
         raise Exception("generate failed with error " + str(status))

      self.log("Running fastsort")
      child = self.runexe(["fastsort", "-i", infile, "-o", outfile],
            status = 0, stderr = "")
      if status != 0:
         raise Failure("fastsort exited with status " + str(status))
      self.log("Fastsort completed in " + str(child.wallclock_time) +
            " seconds")

      self.log("Checking output")
      (is_sorted, msg) = check_sorted(outfile, infile)
      if not is_sorted:
         raise Failure(msg)
      self.done()

class Empty(GenTest):
   name = "empty"
   description = "empty input file"
   timeout = 15
   seed = 0
   n = 0

class One(GenTest):
   name = "one"
   description = "a single record"
   timeout = 15
   seed = 0
   n = 1

class Five1(GenTest):
   name = "five1"
   description = "five records"
   timeout = 15
   seed = 1
   n = 5

class Five2(GenTest):
   name = "five2"
   description = "five records"
   timeout = 15
   seed = 2
   n = 5

class Medium(GenTest):
   name = "medium"
   description = "100 records"
   timeout = 20
   seed = 3
   n = 100

class Large(GenTest):
   name = "large"
   description = "10k records"
   timeout = 30
   seed = 4
   n = 10000

class VeryLarge(GenTest):
   name = "verylarge"
   description = "100k records"
   timeout = 60
   seed = 5
   n = 100000


test_list = [Empty, One, Five1, Five2, Medium, Large, VeryLarge]
