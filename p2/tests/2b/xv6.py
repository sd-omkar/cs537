import pexpect, shutil, os, subprocess

import toolspath
from testing import Test

from build import BuildTest

class Xv6Test(BuildTest, Test):
   name = None
   description = None
   timeout = 30
   tester = "tester.c"
   make_args = ""

   def run(self):
      tester_path = self.test_path + "/" + self.tester
      self.log("Running xv6 user progam " + str(tester_path))
      shutil.copy(tester_path, self.project_path + "/user/tester.c")

      is_success = self.make(["xv6.img", "fs.img"])
      if not is_success:
         return # stop test on if make fails

      target = "qemu-nox " + self.make_args
      if self.use_gdb:
         target = "qemu-gdb " + self.make_args
      self.log("make " + target)
      child = pexpect.spawn("make " + target,
            cwd=self.project_path,
            logfile=self.logfd,
            timeout=None)

      if self.use_gdb:
         subprocess.Popen(["xterm", "-title", "\"gdb\"", "-e", "gdb"],
                  cwd=self.project_path)

      child.expect_exact("init: starting sh")
      child.expect_exact("$ ")
      child.sendline("tester")

      # check if test passed
      index = child.expect_exact(["TEST PASSED", "$ "])
      if index == 1:
         self.fail("tester failed")

      if self.use_gdb:
         child.wait()
      else:
         child.close()
      self.done()

class Xv6Build(BuildTest):
   name = "build"
   description = "build xv6 using make"
   timeout = 60
   targets = ["xv6.img", "fs.img"]


