import toolspath
from testing import BuildTest

tester_files = ["shutdown", "creat", "write", "dirs", "complex"]

build_test = BuildTest
build_test.targets = ["server", "libmfs.so"]

all_tests = list()
for f in tester_files:
  module = __import__(f)
  all_tests.extend(module.test_list)

from testing.runtests import main
main(build_test, all_tests)
