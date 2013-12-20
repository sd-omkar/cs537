import toolspath
from testing.test import *

class ProhibitTest(Test):
   name = "prohibit"
   description = "check for prohibited library functions"
   timelimit = None
   prohibited = [ "malloc", "calloc", "realloc", "posix_memalign",
         "valloc", "memalign"]
   target = "libmem.so"

   def run(self):
      symbols = self.run_filter(["objdump", "--dynamic-syms", self.target])
      for sym in self.prohibited:
         if (symbols.count(sym) > 0):
            raise Failure("Use of '" + sym + "' is not allowed",
                  "malloc and related functions are not allowed in the library")
      self.done()

test_list = [ProhibitTest]
