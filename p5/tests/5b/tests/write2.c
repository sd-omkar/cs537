#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"


int ppid;
#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

int
main(int argc, char *argv[])
{
   ppid = getpid();
   int fd;
   int size = 512;
   int n = (MAXFILE * BSIZE) / size;
   int i, j;
   char buf[size];
   int r;

   printf(1, "buffer size: %d\n", size);
   printf(1, "file size: %d\n", n * size);

   printf(1, "create checked file\n");
   fd = open("out", O_CREATE | O_CHECKED | O_RDWR);
   assert(fd >= 0);

   memset(buf, 0, size);

   printf(1, "writing file\n");
   for (i = 0; i < n; i++) {
      buf[0] = (char)('A' + i);
      r = write(fd, buf, size);
      assert(r == size);
   }

   printf(1, "reopening read only\n");
   r = close(fd);
   assert(r == 0);
   fd = open("out", O_RDONLY);
   assert(fd >= 0);

   printf(1, "reading file\n");
   for (i = 0; i < n; i++) {
      r = read(fd, buf, size);
      assert(r == size);
      assert(buf[0] == (char)('A' + i));
      for (j = 1; j < size; j++) {
         assert(buf[j] == 0);
      }
   }

   printf(1, "TEST PASSED\n");

   exit();
}
