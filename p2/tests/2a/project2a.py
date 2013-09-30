import os, subprocess, shutil

import toolspath
from testing import Test

from build import BuildTest

def readall(filename):
  f = open(filename, 'r')
  s = f.read()
  f.close()
  return s

class ShellBuildTest(BuildTest):
  targets = ['mysh']

class ShellTest(Test):
  def run(self, command = None, stdout = None, stderr = None, addl_args = []):
    batch = self.test_path + '/' + self.name + '/in'
    if command == None:
      command = ['./mysh', batch]
    if stdout == None:
      stdout = readall(self.test_path + '/' + self.name + '/out')
    if stderr == None:
      stderr = readall(self.test_path + '/' + self.name + '/err')

    self.runexe(command, status = self.status, stderr = stderr, stdout = stdout)
    self.done()

class Basic(ShellTest):
  name = 'basic'
  description = 'run a single command with no arguments'
  timeout = 5
  status = 0

class Blank(ShellTest):
  name = 'blank'
  description = 'an empty batch file'
  timeout = 5
  status = 0

class Multiline(ShellTest):
  name = 'multiline'
  description = 'two single line commands'
  timeout = 10
  status = 0

class ParseArgs(ShellTest):
  name = 'parseargs'
  description = 'a command with an argument'
  timeout = 10
  status = 0

class ParseArgs2(ShellTest):
  name = 'parseargs2'
  description = 'a command with many arguments'
  timeout = 10
  status = 0

class ParseArgs3(ShellTest):
  name = 'parseargs3'
  description = 'successfully parse indirection and background symbols'
  timeout = 10
  status = 0

class Exit(ShellTest):
  name = 'exit'
  description = 'the builtin exit command'
  timeout = 10
  status = 0

class PWD(ShellTest):
  name = 'pwd'
  description = 'the builtin pwd command'
  timeout = 10
  status = 0
  def run(self):
    super(PWD, self).run(stdout = 'pwd\n' + self.project_path + '\n')

class CD(ShellTest):
  name = 'cd'
  description = 'cd with argument'
  timeout = 10
  status = 0

class CD2(ShellTest):
  name = 'cd2'
  description = 'cd without argument'
  timeout = 10
  status = 0
  def run(self):
    lsout = subprocess.Popen(['/bin/ls', os.environ['HOME']], 
        stdout=subprocess.PIPE).communicate()[0]
    super(CD2, self).run(stdout = 'cd\nls\n' + lsout)

class Whitespace(ShellTest):
  name = 'whitespace'
  description = 'serveral commands with extra whitespace'
  timeout = 10
  status = 0

class Whitespace2(ShellTest):
  name = 'whitespace2'
  description = 'builtin commands with extra whitespace'
  timeout = 10
  status = 0

class Args(ShellTest):
  name = 'args'
  description = 'too many arguments to mysh'
  timeout = 10
  status = 1
  def run(self):
    batch = self.test_path + '/' + self.name + '/in'
    super(Args, self).run(command = ['./mysh', batch, '/bogus'])


class Args2(ShellTest):
  name = 'args2'
  description = 'invalid batch file path'
  timeout = 10
  status = 1
  def run(self):
    super(Args2, self).run(command = ['./mysh', '/bogus'])

class LongLine(ShellTest):
  name = 'longline'
  description = 'maximum length line'
  timeout = 10
  status = 0

class TooLong(ShellTest):
  name = 'toolong'
  description = 'a line that is too long'
  timeout = 10
  status = 0

class BadProg(ShellTest):
  name = 'badprog'
  description = 'bad executable path as command'
  timeout = 10
  status = 0

class Errout(ShellTest):
  name = 'errout'
  description = 'a called program produces error output, but no errors from the shell itself'
  timeout = 10
  status = 0

class BadCD(ShellTest):
  name = 'badcd'
  description = 'invalid path to cd'
  timeout = 10
  status = 0

class BadPWD(ShellTest):
  name = 'badpwd'
  description = 'invalid arguments to pwd'
  timeout = 10
  status = 0

class BadExit(ShellTest):
  name = 'badexit'
  description = 'invalid arguments to exit'
  timeout = 10
  status = 0

class Redirect(ShellTest):
  name = 'redirect'
  description = 'simple redirection using >'
  timeout = 10
  status = 0

class Redirect2(ShellTest):
  name = 'redirect2'
  description = 'redirection and mutiple arguments'
  timeout = 10
  status = 0

class BadRedir(ShellTest):
  name = 'badredir'
  description = 'redirect to invalid path'
  timeout = 10
  status = 0

class BadRedir2(ShellTest):
  name = 'badredir2'
  description = 'redirect to multiple paths'
  timeout = 10
  status = 0

class Interactive(ShellTest):
  name = 'interactive'
  description = 'simple command in interactive mode'
  timeout = 10
  status = 0
  def run(self):
    batch = self.test_path + '/' + self.name + '/in'
    self.log('Running mysh with 1 command: see {0:s}\n'.format(batch))
    self.runexe(['./mysh'],
        status = 0,
        stdin = readall(batch),
        stdout = readall(self.test_path + '/' + self.name + '/out'),
        stderr = readall(self.test_path + '/' + self.name + '/err'))
    self.done()

class Interactive2(ShellTest):
  name = 'interactive2'
  description = 'series of commands in interactive mode'
  timeout = 10
  status = 0
  def run(self):
    batch = self.test_path + '/' + self.name + '/in'
    self.log('Running mysh with several commands: see {0:s}\n'.format(batch))
    self.runexe(['./mysh'],
        status = 0,
        stdin = readall(batch),
        stdout = readall(self.test_path + '/' + self.name + '/out'),
        stderr = readall(self.test_path + '/' + self.name + '/err'))
    self.done()

class Background(ShellTest):
  name = 'background'
  description = 'basic background process'
  timeout = 10
  status = 0

class Background2(ShellTest):
  name = 'background2'
  description = 'background process followed by wait'
  timeout = 10
  status = 0

class Background3(ShellTest):
  name = 'background3'
  description = 'command following wait'
  timeout = 10
  status = 0

class Background4(ShellTest):
  name = 'background4'
  description = 'check that wait waits correctly'
  timeout = 10
  status = 0
  def run(self):
    shutil.copy(self.test_path + '/background4/decho.py', self.project_path)
    super(Background4, self).run()
    os.remove(self.project_path + '/decho.py')

class BadWait(ShellTest):
  name = 'badwait'
  description = 'invalid arguments to wait'
  timeout = 10
  status = 0

class Complex2(ShellTest):
  name = 'complex2'
  description = 'series of commands'
  timeout = 10
  status = 0

class Complex3(ShellTest):
  name = 'complex3'
  description = 'series of commands'
  timeout = 10
  status = 0

class Fail(ShellTest):
  name = 'fail'
  description = 'always fails, used for testing the test script'
  timeout = 10
  status = 0

class MultiCmd(ShellTest):
  name = 'multicmd'
  description = 'multiple commands on one line'
  timeout = 10
  status = 0

class MultiCmd2(ShellTest):
  name = 'multicmd2'
  description = 'multiple commands with spaces'
  timeout = 10
  status = 0

class MultiCmd3(ShellTest):
  name = 'multicmd3'
  description = 'multiple commands with blanks'
  timeout = 10
  status = 0

class WaitNone(ShellTest):
  name = 'waitnone'
  description = 'wait with no background jobs'
  timeout = 10
  status = 0

class Fun(ShellTest):
  name = 'fun'
  description = 'test the fun feature (python script)'
  timeout = 10
  status = 0
  def run(self):
    shutil.copy(self.test_path + '/fun/echo-funtest.py', self.project_path)
    super(Fun, self).run()
    os.remove(self.project_path + '/echo-funtest.py')

all_tests = [
    Basic,
    Blank,
    Multiline,
    ParseArgs,
    ParseArgs2,
    ParseArgs3,
    Exit,
    PWD,
    CD,
    CD2,
    Whitespace,
    Whitespace2,
    Args,
    Args2,
    LongLine,
    TooLong,
    BadProg,
    Errout,
    BadCD,
    BadPWD,
    BadExit,
    Redirect,
    Redirect2,
    BadRedir,
    BadRedir2,
    Interactive,
    Interactive2,
    Background,
    Background2,
    Background3,
    Background4,
    WaitNone,
    BadWait,
    Fun,
    ]

build_test = ShellBuildTest

from testing.runtests import main
main(build_test, all_tests)
