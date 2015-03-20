/* klaar@ida

   pintos -v -k --fs-disk=2 --qemu -p ../examples/parent -a parent -p ../examples/child -a child -- -f -q run parent | grep PASS > result.txt
   grep -c 'Lab 0' result.txt
   grep -c 'Lab 1' result.txt
   grep -c 'Lab 2' result.txt
   grep -c 'Lab 3' result.txt
   
   A test program that calls itself recursively. In the last step of
   the recursion child.c is started. Do not use with large values for
   CHILDREN or DEPTH.

   Shall produce 64 PASS messages, 16 of each, when CHILDREN=4 and DEPTH=3

   CHILDREN^DEPTH=count(PASS) (4^3=64)
*/
#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>

#define CHILDREN 4
#define DEPTH 3

int main(int argc, char* argv[])
{
  int i;
  int pid[CHILDREN];
  int depth = DEPTH - 1;
  char cmd[10];

  if (argc == 2)
    depth = atoi(argv[1]) - 1;
  
  for(i = 0; i < CHILDREN; i++)
  {
    if (depth)
      snprintf(cmd, 10, "parent %i", depth);
    else
      snprintf(cmd, 10, "child %i", i);

    printf("%s\n", cmd);
    pid[i] = exec(cmd);
  }
//  if (depth <= 1)
  {
    for(i = 0; i < CHILDREN; i++)
    {
      wait(pid[i]);
    }
  }
  exit(0);
}
