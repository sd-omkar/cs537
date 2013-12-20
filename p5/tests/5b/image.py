import shutil, os, subprocess

import toolspath
from testing import Test, BuildTest, Xv6Test, pexpect


class ImageTest(BuildTest, Test):
   name = None
   description = None
   timeout = 30
   command = "test"
   before_image = "images/fs.img"
   after_image = None

   def run(self):
      before_image = os.path.join(self.test_path, self.before_image)
      actual_image = os.path.join(self.project_path, "fs.img")

      is_success = self.make(["xv6.img", "fs.img"])
      if not is_success:
         return # stop test on if make fails

      self.log("Using image " + str(before_image))
      shutil.copy(before_image, actual_image)
      os.utime(actual_image, None)

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

      if self.after_image is not None:
         after_image = os.path.join(self.test_path, self.after_image)
         self.log("Comparing actual image with expected image")
         images = [after_image, actual_image]
         size = max([os.path.getsize(x) for x in images])
         fd = [open(x, "rb") for x in images]
         for i in range((size / 512) + 1):
            block = [x.read(512) for x in fd]
            if block[0] != block[1]:
               self.fail("Block " + str(i) +
                     " of fs.img does not match expected")
               break
            elif len(block[0]) == 0:
               break

         [x.close() for x in fd]

      self.done()

class Write1(Xv6Test):
   name = "write1"
   description = "write then read a block"
   tester = "tests/write1.c"
   timeout = 10

class Write2(Xv6Test):
   name = "write2"
   description = "write then read largest possible file"
   tester = "tests/write2.c"
   timeout = 30

class Write3(Xv6Test):
   name = "write3"
   description = "write then read, buffer smaller than one block"
   tester = "tests/write3.c"
   timeout = 35

class Write4(Xv6Test):
   name = "write4"
   description = "write then read, buffer larger than one block"
   tester = "tests/write4.c"
   timeout = 30

class Write5(ImageTest):
   name = "write5"
   description = "check the correct image is produced after writing"
   timeout = 30
   command = "write5"
   before_image = "images/write5-before.img"
   after_image = "images/write5-after.img"

class Checksum1(Xv6Test):
   name = "checksum1"
   description = "check that stat gives the correct checksum"
   tester = "tests/checksum1.c"

class Checksum2(ImageTest):
   name = "checksum2"
   description = "check that stat gives the correct checksum"
   before_image = "images/checksum2.img"
   command = "checksum2"


test_list = [Write1, Write2, Write3, Write4, Write5, Checksum1, Checksum2]

