tester_files = ["rand", "prohibit"]
import sys, os, inspect

import toolspath
from testing import BuildTest

from ctest import CTest

curdir = os.path.realpath(os.path.dirname(inspect.getfile(inspect.currentframe())))

all_tests = []
path = os.path.join(curdir, 'tests')
for testname in '''init init2 
                   badinit badinit2
                   doubleinit
                   alloc alloc2 alloc3
                   free free2
                   badfree
                   align align2 align3
                   writeable writeable2
                   bestfit bestfit2
                   worstfit worstfit2
                   firstfit firstfit2
                   nospace
                   coalesce coalesce2 coalesce3 coalesce4'''.split():
  members = {
      'name': testname,
      'path': path,
      'description': CTest.get_description(testname, path),
      'timeout': CTest.timeout
      }
  newclass = type(testname, (CTest,), members)
  setattr(sys.modules[__name__], testname, newclass)
  all_tests.append(newclass)

for f in tester_files:
  module = __import__(f)
  all_tests.extend(module.test_list)

build_test = BuildTest
build_test.targets = ['libmem.so']

from testing.runtests import main
main(build_test, all_tests)
