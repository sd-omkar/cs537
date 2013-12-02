import toolspath
from testing import BuildTest

build_test = BuildTest
build_test.targets = ["server"]

all_tests = list()
import server
all_tests.extend(server.test_list)

from testing.runtests import main
main(build_test, all_tests)
