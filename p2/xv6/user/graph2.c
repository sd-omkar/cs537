// Create a zombie process that 
// must be reparented at exit.

#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int i=0,j=0;
  for(i=0;i<10000;i++)
 {
  for(j=0;j<10000;j++)
  {}
 }
 printf(1,"I AM DONE 2\n\n\n");
 exit();
}
