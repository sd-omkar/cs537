from mfs import *

class ShutdownTest(MfsTest):
   name = "shutdown"
   description = "init server and client then call shutdown"
   timeout = 10

   def run(self):
      self.loadlib()
      self.start_server()
      self.mfs_init("localhost", self.port)
      self.shutdown()
      self.server.wait()
      self.done()

test_list = [ShutdownTest]
