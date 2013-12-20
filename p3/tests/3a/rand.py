import os

import toolspath
from testing import Test

class RandomTest(Test):
   name = "random"
   description = "psuedo-random sequence of allocs and frees"
   timeout = 20
   seed = 0
   n = 10
   write = 0
   def run(self):
      child = self.runexe([os.path.join(self.test_path, "random"),
         str(self.seed), str(self.n), str(self.write)],
         status = 0)
      self.log("time: " + str(child.wallclock_time))
      self.done()

class Random2Test(RandomTest):
   name = "random2"
   seed = 0
   n = 1000
   write = 1

class Random3Test(RandomTest):
   name = "random3"
   seed = 0
   n = 100000
   write = 1

class Random4Test(RandomTest):
   name = "random4"
   seed = 1
   n = 500000
   write = 1

class Random5Test(RandomTest):
   name = "random5"
   seed = 1
   n = 2000000
   write = 0
   timeout = 50


test_list = [RandomTest, Random2Test, Random3Test, Random4Test, Random5Test]
