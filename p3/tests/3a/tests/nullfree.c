/* free a NULL */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   assert(Mem_Free(NULL) == 0);
   assert(Mem_Free(NULL) == 0);
   exit(0);
}
