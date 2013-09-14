from build import BuildTest

class FastsortBuild(BuildTest):
  targets = ["fastsort"]

  def run(self):
    self.clean(self.targets + ["*.o"], required=False)
    if not self.make(self.targets, required=False):
      self.log("Trying to build using gcc")
      status = self.run_util(
          ["gcc", "-O", "-Wall", "-o", "fastsort", "fastsort.c"])
      if status != 0:
        self.fail("gcc returned error " + str(status))
    self.done()
