/* bad argument to Mem_Init */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
   assert(Mem_Init(0) == -1);
   assert(m_error == E_BAD_ARGS);
   exit(0);
}
