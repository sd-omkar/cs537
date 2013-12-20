import os, sys

import toolspath
from testing import Test

class CTest(Test):
   timeout = 15
   path = None
   args = []
   def run(self):
      self.runexe([os.path.join(self.path, self.name)] + self.args, status = 0)
      self.done()
   def __get_attr__(self, name):
      print "get_attr", name
   def __get_state__(self):
      print "get_state"
      return None
   #def __reduce__(self):
      #print "reduce"
      #return repr(self)
   @staticmethod
   def get_description(name, path):
      cfile = os.path.join(path, name + ".c")
      with open(cfile, "r") as f:
         desc = f.readline()
      desc = desc.strip()
      desc = desc[2:]
      if desc[-2:] == "*/":
         desc = desc[:-2]
      return desc.strip()
