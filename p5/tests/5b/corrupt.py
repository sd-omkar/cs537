import shutil, os, subprocess

import toolspath
from testing import Test, BuildTest, pexpect

class CorruptTest(BuildTest, Test):
   name = None
   description = None
   timeout = 30
   command = "test"
   image = "images/fs.img"

   def run(self):
      image_path = os.path.join(self.test_path, self.image)
      image_dest = os.path.join(self.project_path, "fs.img")

      is_success = self.make(["xv6.img", "fs.img"])
      if not is_success:
         return # stop test on if make fails

      self.log("Using image " + str(image_path))
      shutil.copy(image_path, image_dest)
      os.utime(image_dest, None)

      target = "qemu-nox"
      if self.use_gdb:
         target = "qemu-gdb"
      self.log("make " + target)
      child = pexpect.spawn("make " + target,
            cwd=self.project_path,
            logfile=self.logfd,
            timeout=None)
      self.children.append(child)

      if self.use_gdb:
         gdb_child = subprocess.Popen(
               ["xterm", "-title", "\"gdb\"", "-e", "gdb"],
               cwd=self.project_path)
         self.children.append(gdb_child)

      child.expect_exact("init: starting sh")
      child.expect_exact("$ ")
      child.sendline(self.command)

      # check if test passed
      patterns = ["TEST FAILED", "cpu\d: panic: .*\n", "TEST PASSED", "[$][ ]"]
      index = child.expect(patterns)
      if index == 0:
         self.fail("tester failed")
      elif index == 1:
         self.fail("xv6 kernel panic")
      elif index == 3:
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

class Corrupt1(CorruptTest):
   name = "corrupt1"
   description = "first block of file is corrupt"
   image = "images/corrupt1.img"
   command = "corrupt1"

class Corrupt2(CorruptTest):
   name = "corrupt2"
   description = "13th block of file is corrupt"
   image = "images/corrupt2.img"
   command = "corrupt2"

class Corrupt3(CorruptTest):
   name = "corrupt3"
   description = "overwrite corrupt data"
   image = "images/corrupt3.img"
   command = "corrupt3"

test_list = [Corrupt1, Corrupt2]#, Corrupt3]
