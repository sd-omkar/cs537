/* many odd sized allocations */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   assert(Mem_Alloc(1, FIRSTFIT) != NULL);
   assert(Mem_Alloc(5, FIRSTFIT) != NULL);
   assert(Mem_Alloc(14, FIRSTFIT) != NULL);
   assert(Mem_Alloc(8, FIRSTFIT) != NULL);
   assert(Mem_Alloc(1, FIRSTFIT) != NULL);
   assert(Mem_Alloc(4, FIRSTFIT) != NULL);
   assert(Mem_Alloc(9, FIRSTFIT) != NULL);
   assert(Mem_Alloc(33, FIRSTFIT) != NULL);
   assert(Mem_Alloc(55, FIRSTFIT) != NULL);
   exit(0);
}
