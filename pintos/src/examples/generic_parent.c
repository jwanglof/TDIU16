/* klaar@ida

   Will try to start N child-processes, giving each an increasing
   number starting on S as argument to main. Will return S+C where C
   is the number of sucessfully started children.

   Normally used from some parent program.
 */

#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE 64

int main(int argc, char* argv[])
{
  int i;
  char cmd[BUF_SIZE];
  char* child;
  int start;
  int count;

  if (argc != 4)
  {
     printf("Usage: %s CHILD S N\n", argv[0]);
     printf("%s starts N copies of CHILD.\n", argv[0]);
     printf("Each child receives an unique integer I as first parameter in the range S..(S+N-1).\n");
     printf("Returns S + N on success, S + I if only I children could be started.\n");
     return -1;
  }
  
  child = argv[1];
  start = atoi(argv[2]);
  count = atoi(argv[3]);
  
  for(i = 0; i < count; i++)
  {
     snprintf(cmd, BUF_SIZE, "%s %i", child, start + i);
     if (exec(cmd) == -1)
     {
	printf("!! ERROR !!\n");
	printf("Could not start '%s'\n", cmd);
	break;
     }
  }
  exit(start + i);
}
