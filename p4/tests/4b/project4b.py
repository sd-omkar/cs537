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
for testname in '''clone clone2 clone3
                   badclone
                   stack
                   join join2 join3 join4
                   thread thread2
                   multi
                   locks
                   cond cond2 cond3
                   race
                   noexit
                   size
                   '''.split():
  members = {
      'name': testname,
      'tester': 'tests/' + testname + '.c',
      'description': get_description(testname),
      'timeout': 10
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
