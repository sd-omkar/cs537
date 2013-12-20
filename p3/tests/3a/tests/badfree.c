/* freeing a pointer that was not returned by Mem_Alloc */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   void* ptr = Mem_Alloc(8, WORSTFIT);
   assert(ptr != NULL);
   assert(Mem_Free((void*)ptr + 8) == -1);
   assert(m_error == E_BAD_POINTER);
   exit(0);
}

