/* align misaligned requests, don't overlap */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   char *ptr[4];
   assert((ptr[0] = Mem_Alloc(1, FIRSTFIT)) != NULL);
   assert((ptr[1] = Mem_Alloc(12, FIRSTFIT)) != NULL);
   assert((ptr[2] = Mem_Alloc(31, FIRSTFIT)) != NULL);
   assert((ptr[3] = Mem_Alloc(4, FIRSTFIT)) != NULL);
   strcpy(ptr[0], "aaaaaaa");
   strcpy(ptr[1], "bbbbbbbbbbbbbbb");
   strcpy(ptr[2], "ccccccccccccccccccccccccccccccc");
   strcpy(ptr[3], "ddddddd");
   assert(strcmp(ptr[0], "aaaaaaa") == 0);
   assert(strcmp(ptr[1], "bbbbbbbbbbbbbbb") == 0);
   assert(strcmp(ptr[2], "ccccccccccccccccccccccccccccccc") == 0);
   assert(strcmp(ptr[3], "ddddddd") == 0);
   strcpy(ptr[3], "ddddddd");
   strcpy(ptr[2], "ccccccccccccccccccccccccccccccc");
   strcpy(ptr[1], "bbbbbbbbbbbbbbb");
   strcpy(ptr[0], "aaaaaaa");
   assert(strcmp(ptr[0], "aaaaaaa") == 0);
   assert(strcmp(ptr[1], "bbbbbbbbbbbbbbb") == 0);
   assert(strcmp(ptr[2], "ccccccccccccccccccccccccccccccc") == 0);
   assert(strcmp(ptr[3], "ddddddd") == 0);
   assert(Mem_Free(ptr[0]) == 0);
   assert(Mem_Free(ptr[1]) == 0);
   assert(Mem_Free(ptr[2]) == 0);
   assert(Mem_Free(ptr[3]) == 0);
   exit(0);
}
