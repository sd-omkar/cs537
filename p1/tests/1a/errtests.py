from tempfile import mktemp

import toolspath
from testing import Test

# tests for checking correct error behavior

usage_error = "Usage: fastsort -i inputfile -o outputfile\n"

class Arg(Test):
   name = "arg"
   description = "bad arguments"
   timeout = 5
   def run(self):
      self.runexe(["fastsort"],
            stderr = usage_error, status = 1)
      self.done()

class Arg2(Test):
   name = "arg2"
   description = "bad arguments"
   timeout = 5
   def run(self):
      self.runexe(["fastsort", "a", "b", "c", "d"],
            stderr = usage_error, status = 1)
      self.done()

class Badin(Test):
   name = "badin"
   description = "bad input file"
   timeout = 5
   def run(self):
      invalid = mktemp(prefix='/invalid/path/')
      self.runexe(["fastsort", "-i", invalid, "-o", "outfile"],
          stderr = "Error: Cannot open file {0}\n".format(invalid), status = 1)
      self.done()

class Badout(Test):
   name = "badout"
   description = "bad output file"
   timeout = 5
   def run(self):
      infile = self.project_path + "/infile"
      # create a valid (empty) input file
      open(infile, "a").close()
      invalid = mktemp(prefix='/invalid/path/')
      self.runexe(["fastsort", "-i", infile, "-o", invalid],
          stderr = "Error: Cannot open file {0}\n".format(invalid), status = 1)
      self.done()

test_list = [Arg, Arg2, Badin, Badout]
