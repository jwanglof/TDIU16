/* klaar@ida

   pintos -v -k --fs-disk=2 --qemu -p ../examples/longrun_interactive -a interactive -p ../examples/generic_parent -a generic_parent -p ../examples/busy -a dummy -- -f -q run 'interactive 10 50'

   NOTE: pintos have a limit on file-name length!

   Start a lot of processes and let them finish to test if we
   eventually run out of process slots.
   To run the test effectively, if you have a limit on number of
   processes, lower the limit to 5 processes and run this test like
  
    'longrun 5 400'
  
   It will fill (and if it works empty) your process table 400 times
   :-)
  
   Note that some pintos tests requires at least 16 simultaneous
   processes to work, so be sure to increase the limit before running
   pintos tests.
 */

#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_SIMULTANEOUS 50
#define MAX_REPEAT 1000
#define BUF_SIZE 64

int main(int argc, char* argv[])
{
  char cmd[BUF_SIZE];
  unsigned j = 0;
  unsigned simul = 0;
  unsigned repeat = 0;

  if (argc != 3)
  {
    printf("Usage: %s simultaneous repeat\n"
           "Where 'simultaneous' is the number of simultaneous processes to \n"
           "start and wait for, and 'repeat' how many times this should be \n"
           "repeated.\n", argv[0]
      );
    return -1;
  }

  simul = atoi(argv[1]);
  repeat = atoi(argv[2]);

  if (simul > MAX_SIMULTANEOUS)
  {
    printf("This test program is compiled with a limitation to max %d \n"
           "simultaneos processes.\n", MAX_SIMULTANEOUS);
    return -1;
  }
  
  if (repeat > MAX_REPEAT)
  {
    printf("This test program is compiled with a limitation to max %d \n"
           "repetitions.\n", MAX_REPEAT);
    return -1;
  }
  
  printf("Will try to start a total of %d processes in groups of %d\n",
         simul * repeat, simul);
  
  for (j = 0; j < repeat; ++j)
  {
     char buf;
     snprintf(cmd, BUF_SIZE, "generic_parent %s %i %i", "dummy", j*simul, simul);
     wait(exec(cmd));

     printf("Press ENTER to continue...\n");
     read(STDIN_FILENO, &buf, 1);
  }
  return 0;
}
