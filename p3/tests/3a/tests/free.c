/* a simple allocation followed by a free */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   void* ptr = Mem_Alloc(8, FIRSTFIT);
   assert(ptr != NULL);
   assert(Mem_Free(ptr) == 0);
   exit(0);
}
