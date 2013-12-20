from mfs import *

import toolspath
from testing.test import Failure

class Dir1Test(MfsTest):
   name = "dir1"
   description = "check root for dot entries"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      inum = self.lookup(ROOT, ".")
      if inum != ROOT:
         raise Failure("'.' in root should point to root inode")

      inum = self.lookup(ROOT, "..")
      if inum != ROOT:
         raise Failure("'..' in root should point to root inode")

      self.shutdown()
      self.server.wait()
      self.done()

class Dir2Test(MfsTest):
   name = "dir2"
   description = "create a new directory and check it"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_DIRECTORY, "testdir")
      inum = self.lookup(ROOT, "testdir")

      if self.lookup(inum, ".") != inum:
         raise Failure("'.' in directory should point to directory itself")

      if self.lookup(inum, "..") != ROOT:
         raise Failure("'..' in directory should point to parent")

      self.shutdown()
      self.server.wait()
      self.done()


class BadDirTest(MfsTest):
   name = "baddir"
   description = "try to create a file with a file parent inode"
   timeout = 15

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_REGULAR_FILE, "testdir")
      inum = self.lookup(ROOT, "testdir")

      r = self.libmfs.MFS_Creat(inum, MFS_REGULAR_FILE, "testfile")
      if r != -1:
         raise Failure("MFS_Creat should fail if pinum is not a directory")

      self.shutdown()
      self.server.wait()
      self.done()

class BadDir2Test(MfsTest):
   name = "baddir2"
   description = "try to lookup a file with a file parent inode"
   timeout = 15

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_REGULAR_FILE, "testdir")
      inum = self.lookup(ROOT, "testdir")

      r = self.libmfs.MFS_Lookup(inum, "testfile")
      if r != -1:
         raise Failure("MFS_Lookup should fail if pinum is not a directory")

      self.shutdown()
      self.server.wait()
      self.done()

class UnlinkTest(MfsTest):
   name = "unlink"
   description = "unlink a file"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_REGULAR_FILE, "test")
      inum = self.lookup(ROOT, "test")

      self.unlink(ROOT, "test")
      inum = self.libmfs.MFS_Lookup(ROOT, "test")
      if inum != -1:
         raise Failure("MFS_Lookup should fail on an unlinked file")

      self.shutdown()
      self.server.wait()
      self.done()

class Unlink2Test(MfsTest):
   name = "unlink2"
   description = "unlink a directory"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_DIRECTORY, "test")
      inum = self.lookup(ROOT, "test")

      self.unlink(ROOT, "test")
      inum = self.libmfs.MFS_Lookup(ROOT, "test")
      if inum != -1:
         raise Failure("MFS_Lookup should fail on an unlinked file")

      self.shutdown()
      self.server.wait()
      self.done()


class EmptyTest(MfsTest):
   name = "empty"
   description = "unlink a non-empty directory"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_DIRECTORY, "testdir")
      inum = self.lookup(ROOT, "testdir")

      self.creat(inum, MFS_REGULAR_FILE, "testfile")
      self.lookup(inum, "testfile")

      r = self.libmfs.MFS_Unlink(ROOT, "testdir")
      if r != -1:
         raise Failure("MFS_Unlink should fail on non-empty dir")

      self.unlink(inum, "testfile")
      self.unlink(ROOT, "testdir")

      self.shutdown()
      self.server.wait()
      self.done()

class NameTest(MfsTest):
   name = "name"
   description = "name too long"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      toolong = "A" * 60
      r = self.libmfs.MFS_Creat(ROOT, MFS_REGULAR_FILE, toolong)
      if r != -1:
         raise Failure("Name argument too long did not result in failure")

      self.shutdown()
      self.server.wait()
      self.done()



test_list = [Dir1Test, Dir2Test,
      BadDirTest, BadDir2Test, UnlinkTest, Unlink2Test, EmptyTest,
      NameTest]
