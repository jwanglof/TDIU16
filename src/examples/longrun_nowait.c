/* klaar@ida

   pintos -v -k --fs-disk=2 --qemu -p ../examples/longrun_nowait -a nowait -p ../examples/generic_parent -a generic_parent -p ../examples/busy -a dummy -- -f -q run 'nowait 10 50'

   Start a lot of processes and let them finish to test if we
   eventually run out of process slots.

    'longrun_nowait 10 50'

   Will call generic_parent to start 10 children 50 times (500
   processes). One slot will be used by longrun_nowait itself. 50
   slots will be used by generic_parent (they must be kept so long as
   longrun_nowait still executes, since it may want to wait for any of
   them). All other slots needed should also be freed as soon as both
   generic_parent and it's set of children exits.

   To run the test effectively, if you have a limit on number of
   processes, make sure that you have at least 100 slots in your
   process table and run this test like

    'longrun_nowait 10 50'

   You should expect the process list to contain both dummy and
   generic_parent. The ratio of dummy to generic_parent processes
   should be in favor of generic_parent at most times. Otherwise
   something is probably not as it should be.

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
           "start before busywaiting a short while, and 'repeat' how many \n"
           "times this should be repeated.\n", argv[0]
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
    /* you may have to increase the multiple to more than 5 */
    int ticks = 10 * 1000 * 1000 * j / repeat;
    
    snprintf(cmd, BUF_SIZE, "generic_parent %s %i %i", "dummy", j*simul, simul);
    
    exec(cmd);
    
//    plist();

    /* since we do not have the wait systemcall yet */
    printf("Now entering busy-loop to let some processes finish\n");
    while (ticks--)
      ;
  }
  return 0;
}
