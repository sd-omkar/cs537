/* write to a chunk from Mem_Alloc */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   void* ptr = Mem_Alloc(8, FIRSTFIT);
   assert(ptr != NULL);
   *((int*)ptr) = 42;   // check pointer is in a writeable page
   exit(0);
}
