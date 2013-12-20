import os, errno

def process_table():
   table = dict()
   for f in os.listdir("/proc"):
      if f.isdigit():
         pid = int(f)
         stat = ProcessStat(pid)
         table[pid] = stat
   return table

class ProcessTree(object):
   def __init__(self, at = None):
      if at is None:
         self._table = process_table()
         self._roots = list()
         for p in self._table.itervalues():
            p.children = list()
         for p in self._table.itervalues():
            if p.ppid in self._table:
               self._table[p.ppid].children.append(p)
            else:
               self._roots.append(p)
      else:
         self._table = dict()
         self._roots = at
         todo = list(self._roots)
         while len(todo) > 0:
            cur = todo.pop()
            todo.extend(cur.children)
            self._table[cur.pid] = cur

   def __getitem__(self, index):
      return self._table[index]

   @property
   def roots(self):
      return list(self._roots)

   @staticmethod
   def _get_string(node, depth):
      s = "  " * depth
      s += str(node)
      children = [ProcessTree._get_string(child, depth + 1)
            for child in node.children]
      return "\n".join([s] + children)

   def __str__(self):
      return "\n".join([self._get_string(root, 0) for root in self.roots])

   def __iter__(self):
      return self._table.itervalues()

   def tree_at(self, root):
      return ProcessTree([root])

   def kill(self, sig):
      missing = list()
      for p in self:
         try:
            os.kill(p.pid, sig)
         except OSError as e:
            if e.errno == errno.ESRCH:
               missing.append(p)
            else:
               raise e
      return missing

class ProcessStat(object):
   def __init__(self, pid):
      self._pid = pid
      with open(os.path.join("/proc", str(self._pid), "stat"), "r") as f:
         self._statline = f.readline().split()

   @property
   def pid(self):
      return int(self._statline[0])

   @property
   def comm(self):
      return self._statline[1][1:-1]

   @property
   def ppid(self):
      return int(self._statline[3])

   def __repr__(self):
      return self.comm + "(" + str(self.pid) + ")"



