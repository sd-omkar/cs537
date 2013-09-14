from xv6 import Xv6Build
build_test = Xv6Build

from xv6 import Xv6Test

class Basic(Xv6Test):
   name = "basic"
   description = "call getsyscallinfo() from a user program"
   tester = "basic.c"

class Getsyscallinfo(Xv6Test):
   name = "getsyscallinfo"
   description = "repeated calls of getsyscallinfo()"
   tester = "getsyscallinfo.c"

class Many(Xv6Test):
   name = "many"
   description = "count many other syscalls"
   tester = "many.c"

class Usertests(Xv6Test):
   name = "usertests"
   description = "run usertests to check for regressions"
   tester = "usertests.c"
   timeout = 300


def test_list(project_path, test_path, log):
   return [Basic, Getsyscallinfo, Many]
