/* many odd sized allocations and interspersed frees */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   void * ptr[9];
   ptr[0] = Mem_Alloc(1, BESTFIT);
   ptr[1] = (Mem_Alloc(5, BESTFIT));
   ptr[2] = (Mem_Alloc(14, BESTFIT));
   ptr[3] = (Mem_Alloc(8, BESTFIT));

   assert(Mem_Free(ptr[1]) == 0);
   assert(Mem_Free(ptr[0]) == 0);
   assert(Mem_Free(ptr[3]) == 0);

   ptr[4] = (Mem_Alloc(1, BESTFIT));
   ptr[5] = (Mem_Alloc(4, BESTFIT));
   assert(ptr[4] != NULL);
   assert(ptr[5] != NULL);

   assert(Mem_Free(ptr[5]) == 0);

   ptr[6] = (Mem_Alloc(9, BESTFIT));
   ptr[7] = (Mem_Alloc(33, BESTFIT));
   assert(ptr[6] != NULL);
   assert(ptr[7] != NULL);

   assert(Mem_Free(ptr[4]) == 0);

   ptr[8] = (Mem_Alloc(55, BESTFIT));
   assert(ptr[8] != NULL);

   assert(Mem_Free(ptr[2]) == 0);
   assert(Mem_Free(ptr[7]) == 0);
   assert(Mem_Free(ptr[8]) == 0);
   assert(Mem_Free(ptr[6]) == 0);

   exit(0);
}
