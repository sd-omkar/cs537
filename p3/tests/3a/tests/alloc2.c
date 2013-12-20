/* a few aligned allocations */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   assert(Mem_Alloc(8, FIRSTFIT) != NULL);
   assert(Mem_Alloc(16, FIRSTFIT) != NULL);
   assert(Mem_Alloc(32, FIRSTFIT) != NULL);
   assert(Mem_Alloc(8, FIRSTFIT) != NULL);
   exit(0);
}
