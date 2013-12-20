from mfs import *

class CreatTest(MfsTest):
   name = "creat"
   description = "creat a file and check with lookup"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)
      self.creat(0, MFS_REGULAR_FILE, "test")
      inum = self.lookup(0, "test")
      self.shutdown()
      self.server.wait()
      self.done()

test_list = [CreatTest]
