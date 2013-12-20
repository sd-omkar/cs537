/* call init twice */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   assert(Mem_Init(4096) == -1);
   //assert(m_error == E_BAD_ARGS);
   exit(0);
}
