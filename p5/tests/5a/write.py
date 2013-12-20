from mfs import *

import toolspath
from testing.test import Failure

class WriteTest(MfsTest):
   name = "write"
   description = "write then read one block"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()

      self.mfs_init("localhost", self.port)
      self.creat(0, MFS_REGULAR_FILE, "test")
      inum = self.lookup(0, "test")

      buf1 = gen_block(1)
      self.write(inum, buf1, 0)

      buf2 = BlockBuffer()
      self.read(inum, buf2, 0)

      print buf1[:20], buf1[-20:]
      print buf2[:20], buf2[-20:]
      if not bufs_equal(buf1, buf2):
         raise Failure("Corrupt data returned by read")

      self.shutdown()

      self.server.wait()
      self.done()

class StatTest(MfsTest):
   name = "stat"
   description = "stat a regular file"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()

      self.mfs_init("localhost", self.port)
      self.creat(ROOT, MFS_REGULAR_FILE, "test")
      inum = self.lookup(ROOT, "test")

      st = self.stat(ROOT)
      if st.type != MFS_DIRECTORY:
         raise Failure("Stat gave wrong type")

      st = self.stat(inum)
      if st.size != 0:
         raise Failure("Stat gave wrong size")
      if st.type != MFS_REGULAR_FILE:
         raise Failure("Stat gave wrong type")

      buf1 = gen_block(1)
      self.write(inum, buf1, 0)

      st = self.stat(inum)
      if st.size != MFS_BLOCK_SIZE:
         raise Failure("Stat gave wrong size")
      if st.type != MFS_REGULAR_FILE:
         raise Failure("Stat gave wrong type")

      self.shutdown()
      self.server.wait()
      self.done()


class OverwriteTest(MfsTest):
   name = "overwrite"
   description = "overwrite a block"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()

      self.mfs_init("localhost", self.port)
      self.creat(0, MFS_REGULAR_FILE, "test")
      inum = self.lookup(0, "test")

      buf1 = gen_block(1)
      self.write(inum, buf1, 0)
      self.read_and_check(inum, 0, buf1)

      buf2 = gen_block(2)
      self.write(inum, buf2, 0)
      self.read_and_check(inum, 0, buf2)

      self.shutdown()

      self.server.wait()
      self.done()

class MaxFileTest(MfsTest):
   name = "maxfile"
   description = "write largest possible file"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()

      self.mfs_init("localhost", self.port)
      self.creat(0, MFS_REGULAR_FILE, "test")
      inum = self.lookup(0, "test")

      buf = [gen_block(i) for i in range(MAX_FILE_BLOCKS)]

      for i in range(MAX_FILE_BLOCKS):
         self.write(inum, buf[i], i)

      for i in range(MAX_FILE_BLOCKS):
         self.read_and_check(inum, i, buf[i])

      self.shutdown()

      self.server.wait()
      self.done()


class MaxFile2Test(MfsTest):
   name = "maxfile2"
   description = "write more blocks than possible"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)
      self.creat(0, MFS_REGULAR_FILE, "test")
      inum = self.lookup(0, "test")

      buf = [gen_block(i) for i in range(MAX_FILE_BLOCKS + 1)]
      for i in range(MAX_FILE_BLOCKS):
         self.write(inum, buf[i], i)
      i = MAX_FILE_BLOCKS
      r = self.libmfs.MFS_Write(inum, byref(buf[i]), i)
      if r != -1:
         raise Failure("MFS_Write should fail on inalid block number")

      for i in range(MAX_FILE_BLOCKS):
         self.read_and_check(inum, i, buf[i])
      i = MAX_FILE_BLOCKS
      r = self.libmfs.MFS_Read(inum, byref(buf[i]), i)
      if r != -1:
         raise Failure("MFS_Read should fail on inalid block number")

      self.shutdown()

      self.server.wait()
      self.done()

class SparseTest(MfsTest):
   name = "sparse"
   description = "write first and last block"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()

      self.mfs_init("localhost", self.port)
      self.creat(0, MFS_REGULAR_FILE, "test")
      inum = self.lookup(ROOT, "test")

      buf = gen_block(1)
      self.write(inum, buf, 0)
      self.read_and_check(inum, 0, buf)

      buf = gen_block(2)
      self.write(inum, buf, MAX_FILE_BLOCKS - 1)
      self.read_and_check(inum, MAX_FILE_BLOCKS - 1, buf)

      buf = gen_block(3)
      self.write(inum, buf, 0)
      self.read_and_check(inum, 0, buf)
      buf = gen_block(2)
      self.read_and_check(inum, MAX_FILE_BLOCKS - 1, buf)

      buf = gen_block(4)
      self.write(inum, buf, MAX_FILE_BLOCKS - 1)
      self.read_and_check(inum, MAX_FILE_BLOCKS - 1, buf)
      buf = gen_block(3)
      self.read_and_check(inum, 0, buf)

      self.shutdown()
      self.server.wait()
      self.done()

class Stat2Test(MfsTest):
   name = "stat2"
   description = "stat a sparse file"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()

      self.mfs_init("localhost", self.port)
      self.creat(0, MFS_REGULAR_FILE, "test")
      inum = self.lookup(ROOT, "test")

      buf = gen_block(1)
      self.write(inum, buf, 0)
      buf = gen_block(2)
      self.write(inum, buf, MAX_FILE_BLOCKS - 1)
      buf = gen_block(3)
      self.write(inum, buf, 0)
      buf = gen_block(4)
      self.write(inum, buf, MAX_FILE_BLOCKS - 1)

      st = self.stat(inum)
      if st.type != MFS_REGULAR_FILE:
         raise Failure("Stat gave wrong type")
      if st.size != MAX_FILE_BLOCKS * MFS_BLOCK_SIZE:
         raise Failure("Stat gave wrong size")

      self.shutdown()
      self.server.wait()
      self.done()





test_list = [WriteTest, StatTest, OverwriteTest, MaxFileTest, MaxFile2Test, SparseTest, Stat2Test]
