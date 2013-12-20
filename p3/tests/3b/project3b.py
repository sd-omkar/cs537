import sys, os, inspect

import toolspath
from testing import Xv6Test, Xv6Build

curdir = os.path.realpath(os.path.dirname(inspect.getfile(inspect.currentframe())))
def get_description(name):
  cfile = os.path.join(curdir, 'tests', name+'.c')
  with open(cfile, 'r') as f:
    desc = f.readline()
  desc = desc.strip()
  desc = desc[2:]
  if desc[-2:] == '*/':
    desc = desc[:-2]
  return desc.strip()

all_tests = []
build_test = Xv6Build
for testname in '''null null2
                   stack stack2 stack3 stack4
                   heap heap2
                   bounds bounds2 bounds3 bounds4
                   '''.split():
  members = {
      'name': testname,
      'tester': 'tests/' + testname + '.c',
      'description': get_description(testname)
      }
  newclass = type(testname, (Xv6Test,), members)
  all_tests.append(newclass)
  setattr(sys.modules[__name__], testname, newclass)

class usertests(Xv6Test):
  name = 'usertests'
  tester = 'tests/usertests.c'
  description = get_description('usertests')
  timeout = 240

#all_tests.append(usertests)

from testing.runtests import main
main(build_test, all_tests)
