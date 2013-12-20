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
   int size = 1000;
   int n = (MAXFILE * BSIZE) / size;
   int i;
   char buf[size];
   int r;

   if (n % 2 != 0)
      n--;
   printf(1, "buffer size: %d\n", size);
   printf(1, "file size: %d\n", n * size);

   printf(1, "create checked file\n");
   fd = open("out", O_CREATE | O_CHECKED | O_RDWR);
   assert(fd >= 0);

   memset(buf, 0, size);
   strcpy(buf, "BLOCK ");

   unsigned char checksum = 0;

   printf(1, "writing file\n");
   for (i = 0; i < n; i++) {
      buf[6] = (char)('A' + i);
      checksum ^= buf[6];
      r = write(fd, buf, size);
      assert(r == size);
   }

   struct stat st;
   fstat(fd, &st);
   printf(1, "stat checksum: %d actual: %d\n", (int)st.checksum, (int)checksum);
   assert(st.checksum == checksum);

   r = close(fd);
   assert(r == 0);

   printf(1, "TEST PASSED\n");

   exit();
}
