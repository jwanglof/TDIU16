/* klaar@ida

   Just do some work, and then exit with status given by first
   argument.

   Normally called by some parent test program.
 */

#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char* argv[])
{
  int i;
  
  if (argc != 2)
    return 0;
  
  for(i = 0; i < 200000; i++)
  {
    int a = (i * i) + (i * i);
    int b = i;
    i = a; a = b; i = b;
  }
  return atoi(argv[1]);
}
