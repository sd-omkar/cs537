from collections import deque
import shutil, os
from mfs import *

import toolspath
from testing.build import *
from testing.test import Test, Failure

class WriteAction:
   def __init__(self, inum, block, data):
      self.inum = inum
      self.block = block
      self.data = data
   def do(self, mfs):
      mfs.write(self.inum, gen_block(self.data), self.block)
      return list()

class File:
   def __init__(self, type, name):
      self.name = name
      self.type = type
      self.isroot = False
   def fullpath(self):
      if self.isroot:
         return ""
      else:
         return self.parent.fullpath() + "/" + self.name
   def do(self, mfs):
      if not self.isroot:
         mfs.creat(self.parent.inum, self.type, self.name)
         self.inum = mfs.lookup(self.parent.inum, self.name)
      return list()

class RegularFile(File):
   def __init__(self, name, blocks):
      File.__init__(self, MFS_REGULAR_FILE, name)
      self.blocks = blocks
      while len(self.blocks) > 0 and self.blocks[-1] is None:
         self.blocks.pop()
   def size(self):
      return len(self.blocks) * MFS_BLOCK_SIZE
   def do(self, mfs):
      File.do(self, mfs)
      actions = list()
      for i in range(len(self.blocks)):
         if self.blocks[i] is not None:
            actions.append(WriteAction(self.inum, i, self.blocks[i]))
      return actions
   def check(self, mfs):
      mfs.log("checking " + self.fullpath() + "\n")
      for i in range(len(self.blocks)):
         if self.blocks[i] is not None:
            mfs.read_and_check(self.inum, i, gen_block(self.blocks[i]))
      st = mfs.stat(self.inum)
      if st.size != self.size():
         raise Failure("Incorrect size for file " + self.fullpath())

   def __str__(self):
      return self.fullpath() + " " + str(self.blocks)

class Dir(File):
   @staticmethod
   def root():
      r = Dir("")
      r.isroot = True
      r.inum = ROOT
      return r
   def __init__(self, name):
      File.__init__(self, MFS_DIRECTORY, name)
      self.name = name
      self.children = dict()
   def add_child(self, child):
      if len(self.children) >= MAX_FILES_PER_DIR:
         return None
      self.children[child.name] = child
      child.parent = self
      return child
   def do(self, mfs):
      File.do(self, mfs)
      return self.children.values()
   def check(self, mfs):
      for child in self.children.values():
         child_inum = mfs.lookup(self.inum, child.name)
         if child_inum != child.inum:
            raise Failure("Incorrect inum for " + child.fullpath())
         child.check(mfs)
   def __str__(self):
      return "\n".join([self.fullpath()] +
            [str(child) for child in self.children.values()])

#def randomFile(rand):
#  f = RegularFile(

class ComplexTest(MfsTest):
   name = "complex"
   description = "a long sequence of creats and writes followed by a check"
   timeout = 30
   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      # bulid a filesystem
      fs = Dir.root()
      for i in range(3):
         a = fs.add_child(Dir(str(i)))
         a.add_child(RegularFile("file1", [i]))
         a.add_child(RegularFile("file2", [i]))
         for j in range(3):
            b = a.add_child(Dir(str(j)))
            b.add_child(RegularFile("file1", [i + j]))
            b.add_child(RegularFile("file2", [i, None, j]))
      self.log("Filesyste to be written:\n")
      self.log(str(fs) + "\n")

      # run commands to creat dirs and write to files
      q = deque([fs])
      while len(q) > 0:
         q.extend(q.popleft().do(self))

      # check full filesystem including all file sizes and data
      fs.check(self)

      self.shutdown()
      self.server.wait()
      self.done()


class Complex2Test(MfsTest):
   name = "complex2"
   description = "a long sequence of creats and writes followed by a check"
   timeout = 30
   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      # bulid a filesystem
      fs = Dir.root()
      for i in range(4):
         a = fs.add_child(Dir(str(i)))
         a.add_child(RegularFile("file1", [i]))
         a.add_child(RegularFile("file2", [2*i]))
         for j in range(3):
            b = a.add_child(Dir(str(j)))
            b.add_child(RegularFile("file1", [i + j]))
            b.add_child(RegularFile("file2", [i, None, j]))
            for k in range(2):
               c = b.add_child(Dir(str(k)))
               c.add_child(RegularFile("file1", [i+j+k]))
               c.add_child(RegularFile("file2", [i, None, j, None, k]))
      self.log("Filesyste to be written:\n")
      self.log(str(fs) + "\n")

      # run commands to creat dirs and write to files
      q = deque([fs])
      while len(q) > 0:
         q.extend(q.popleft().do(self))

      # check full filesystem including all file sizes and data
      fs.check(self)

      self.shutdown()
      self.server.wait()
      self.done()

class PersistTest(MfsTest):
   name = "persist"
   description = "restart server after creating a file"
   timeout = 30
   def run(self):
      image = "testimage"
      if os.path.exists(self.project_path + "/" + image):
         os.remove(self.project_path + "/" + image)

      self.loadlib()
      self.start_server(image)
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_REGULAR_FILE, "test")
      inum = self.lookup(ROOT, "test")
      self.write(inum, gen_block(1), 0)

      if self.lookup(ROOT, "test") != inum:
         raise Failure("Wrong inum")
      self.read_and_check(inum, 0, gen_block(1))

      self.shutdown()
      self.server.wait()

      #self.mfs_init("localhost", self.port)
      self.start_server(image, port=self.port)
      if self.lookup(ROOT, "test") != inum:
         raise Failure("Wrong inum")
      self.read_and_check(inum, 0, gen_block(1))

      self.shutdown()
      self.server.wait()
      self.done()

class Persist2Test(MfsTest):
   name = "persist2"
   description = "restart server after creating many dirs and files"
   timeout = 30
   def run(self):
      image = "testimage"
      if os.path.exists(self.project_path + "/" + image):
         os.remove(self.project_path + "/" + image)

      self.loadlib()
      self.start_server(image)
      self.mfs_init("localhost", self.port)

      # bulid a filesystem
      fs = Dir.root()
      for i in range(4):
         a = fs.add_child(Dir(str(i)))
         a.add_child(RegularFile("file1", [i]))
         a.add_child(RegularFile("file2", [2*i]))
         for j in range(3):
            b = a.add_child(Dir(str(j)))
            b.add_child(RegularFile("file1", [i + j]))
            b.add_child(RegularFile("file2", [i, None, j]))
            for k in range(2):
               c = b.add_child(Dir(str(k)))
               c.add_child(RegularFile("file1", [i+j+k]))
               c.add_child(RegularFile("file2", [i, None, j, None, k]))
      self.log("Filesyste to be written:\n")
      self.log(str(fs) + "\n")

      # run commands to creat dirs and write to files
      q = deque([fs])
      while len(q) > 0:
         q.extend(q.popleft().do(self))

      # check full filesystem including all file sizes and data
      fs.check(self)

      self.shutdown()
      self.server.wait()

      self.start_server(image, port=self.port)
      #self.mfs_init("localhost", self.port)
      fs.check(self)

      self.shutdown()
      self.server.wait()
      self.done()


class DropTest(MfsTest, BuildTest):
   name = "drop"
   description = "drop every other packet sent by client"
   timeout = 90

   def run(self):
      self.loadlib()

      os.rename(self.project_path + "/udp.c", self.project_path + "/udp.c.old")
      shutil.copy(self.test_path + "/udp_drop.c", self.project_path + "/udp.c")
      self.clean(self.targets + ["*.o"])
      self.make(["server"])

      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_REGULAR_FILE, "test")
      inum = self.lookup(ROOT, "test")
      self.write(inum, gen_block(1), 0)
      self.read_and_check(inum, 0, gen_block(1))

      # shutdown not reliable when dropping packets
      self.server.kill()
      self.done()

   def after(self):
      shutil.copy(self.test_path + "/udp_orig.c", self.project_path + "/udp.c")
      self.clean(self.targets + ["*.o"])
      self.make(["server"])

class BigDirTest(MfsTest):
   name = "bigdir"
   description = "create a directory with max number of files"
   timeout = 180

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_DIRECTORY, "testdir")
      inum = self.lookup(ROOT, "testdir")

      for i in range(MAX_FILES_PER_DIR):
         self.creat(inum, MFS_REGULAR_FILE, str(i))

      for i in range(MAX_FILES_PER_DIR):
         self.lookup(inum, str(i))

      self.shutdown()
      self.server.wait()
      self.done()

class BigDir2Test(MfsTest):
   name = "bigdir2"
   description = "create a directory with more files than possible"
   timeout = 180

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_DIRECTORY, "testdir")
      inum = self.lookup(ROOT, "testdir")

      for i in range(MAX_FILES_PER_DIR):
         self.creat(inum, MFS_REGULAR_FILE, str(i))

      i = MAX_FILES_PER_DIR
      r = self.libmfs.MFS_Creat(inum, MFS_REGULAR_FILE, str(i))
      if r != -1:
         raise Failure("MFS_Creat should fail if directory is full")

      for i in range(MAX_FILES_PER_DIR):
         self.lookup(inum, str(i))

      self.shutdown()
      self.server.wait()
      self.done()

class DeepTest(MfsTest):
   name = "deep"
   description = "create many deeply nested directories"
   timeout = 60

   depth = 200

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      self.creat(ROOT, MFS_DIRECTORY, "testdir")
      inum = self.lookup(ROOT, "testdir")

      for i in range(self.depth):
         self.creat(inum, MFS_DIRECTORY, str(i))
         inum = self.lookup(inum, str(i))

      inum = self.lookup(ROOT, "testdir")
      for i in range(self.depth):
         inum = self.lookup(inum, str(i))

      self.shutdown()
      self.server.wait()
      self.done()

class FreeTest(MfsTest):
   name = "free"
   description = "check that inodes and direcrtory entries are freed"
   timeout = 900

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)

      for i in range(MAX_INODES + 10):
         self.creat(ROOT, MFS_REGULAR_FILE, str(i))
         self.unlink(ROOT, str(i))
      self.creat(ROOT, MFS_REGULAR_FILE, "test")
      self.lookup(ROOT, "test")

      self.shutdown()
      self.server.wait()
      self.done()


test_list = [PersistTest, DropTest, BigDirTest, BigDir2Test, DeepTest,
      ComplexTest, Complex2Test, Persist2Test, FreeTest]
