/* a few allocations checked for alignment */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   void* ptr[4];

   ptr[0] = Mem_Alloc(8, FIRSTFIT);
   ptr[1] = Mem_Alloc(16, FIRSTFIT);
   ptr[2] = Mem_Alloc(32, FIRSTFIT);
   ptr[3] = Mem_Alloc(8, FIRSTFIT);

   assert((uintptr_t)(ptr[0]) % 8 == 0);
   assert((uintptr_t)(ptr[1]) % 8 == 0);
   assert((uintptr_t)(ptr[2]) % 8 == 0);
   assert((uintptr_t)(ptr[3]) % 8 == 0);

   exit(0);
}
