#! /usr/bin/env python
import sys, struct
#from collections import Counter

reclen = 100
min_integer = -sys.maxint - 1

class Counter(dict):
    def __init__(self, iterable=None, **kwds):
        super(Counter, self).__init__()
        self.update(iterable, **kwds)

    def __missing__(self, key):
        'The count of elements not in the Counter is zero.'
        # Needed so that self[missing_item] does not raise KeyError
        return 0

    def elements(self):
        return _chain.from_iterable(_starmap(_repeat, self.iteritems()))

    @classmethod
    def fromkeys(cls, iterable, v=None):
        # There is no equivalent method for counters because setting v=1
        # means that no element can have a count greater than one.
        raise NotImplementedError(
            'Counter.fromkeys() is undefined.  Use Counter(iterable) instead.')

    def update(self, iterable=None, **kwds):
        if iterable is not None:
            if isinstance(iterable, Mapping):
                if self:
                    self_get = self.get
                    for elem, count in iterable.iteritems():
                        self[elem] = self_get(elem, 0) + count
                else:
                    super(Counter, self).update(iterable)
            else:
                self_get = self.get
                for elem in iterable:
                    self[elem] = self_get(elem, 0) + 1
        if kwds:
            self.update(kwds)

    def __reduce__(self):
        return self.__class__, (dict(self),)

    def __delitem__(self, elem):
        'Like dict.__delitem__() but does not raise KeyError for missing values.'
        if elem in self:
            super(Counter, self).__delitem__(elem)

    def __repr__(self):
        if not self:
            return '%s()' % self.__class__.__name__
        items = ', '.join(map('%r: %r'.__mod__, self.most_common()))
        return '%s({%s})' % (self.__class__.__name__, items)


# get integer key from record
def get_key(rec):
   (key,) = struct.unpack("i", rec[:4])
   return key

# return a printable list of records, possibly truncated
def trunc_list(l):
   n = 3
   if len(l) <= n:
      return ", ".join([str(get_key(i)) for i in l])
   return ", ".join([str(get_key(i)) for i in l[:n]] + ["..."])

# check that sorted_file is sorted and contains the same records as orig_file
def check_sorted(sorted_file, orig_file):
   count = Counter()

   with open(orig_file, "rb") as f:
      while True:
         rec = f.read(reclen)
         if len(rec) == 0:
            break
         count[rec] += 1

   prevkey = min_integer
   with open(sorted_file, "rb") as f:
      while True:
         rec = f.read(reclen)
         if len(rec) == 0:
            break
         key = get_key(rec)
         if key < prevkey:
            return (False, "File not sorted, " +
                  "key " + str(prevkey) + " appears before key " + str(key))
         count[rec] -= 1
         if count[rec] == 0:
            del count[rec]
         prevkey = key

   if len(count) > 0:
      missing = [rec for rec, n in count.iteritems() if n > 0]
      extra = [rec for rec, n in count.iteritems() if n < 0]
      r = []
      if len(missing) > 0:
         r.append("missing " + str(len(missing)) + " records "
               + "(" + trunc_list(missing) + ")")
      if len(extra) > 0:
         r.append("extra " + str(len(extra)) + " records "
               + "(" + trunc_list(extra) + ")")
      return (False, "\n".join(r))

   return (True, "File is sorted and contains all records")

def main():
   if len(sys.argv) != 3:
      print "Usage: check.py sorted_file original_file"
      sys.exit(2)
   (is_sorted, msg) = check_sorted(sys.argv[1], sys.argv[2])
   print msg
   if is_sorted:
      sys.exit(0)
   else:
      sys.exit(1)

if __name__ == "__main__":
    main()
