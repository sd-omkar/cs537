from creat import *

import toolspath
from testing import Test

class StraceTest(Test):

   def startexe(self, name, args, libs = None):
      path = self.project_path + "/" + name
      st_args = (["-o", self.project_path + "/" + name +".strace"] +
            [path] + args)
      print st_args
      return Test.startexe(self, "strace", st_args, libs, path="strace")


class CreatStraceTest(StraceTest, CreatTest):
   name = "seek"
   description = ""
   timeout = 20

   def run(self):
      CreatTest.run(self)
      f = open(self.project_path + "/server.strace", "r")
      for line in f:
         print line

test_list = [CreatStraceTest]
