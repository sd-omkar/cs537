import toolspath
from testing import Xv6Test, Xv6Build

class UsertestsTest(Xv6Test):
   name = "usertests"
   description = "check for regressions"
   tester = "usertests.c"
   timeout = 300

all_tests = list()
import image, corrupt
all_tests.extend(image.test_list)
all_tests.extend(corrupt.test_list)
#all_tests.append(UsertestsTest)

build_test = Xv6Build

from testing.runtests import main
main(build_test, all_tests)
