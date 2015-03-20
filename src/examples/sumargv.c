/* klaar@ida
    
   A test program for the stack setup that do not rely on any (they
   are not yet implemented!) system calls.
 
   The intended pintos command line running from userprog is:
   
   pintos -v -k --fs-disk=2 -p ../examples/sumargv -a sumargv -- -f -q run 'sumargv'
   
   This invocation shall return 0. To return any other number, invoke
   with numbers that sum to that number. 'sumargv 1000 200 4 30 will
   result in 1234.
   
   Note -v (no vga) and -q (poweroff when done) may cause trouble. If
   you have problems exiting pintos, pres C-a and then x. If the
   program does not seem to work it may be due to -q do poweroff too
   fast (process_wait is incorrect).
 */
#include <stdlib.h>

int
main (int argc, char **argv)
{
  int i;
  int sum = 0;
  char* argv_me = "sumargv";
  char* p;

  /* If we do not have a valid `argv', exit with error code 111. */
  if (argv == 0)
    return 111;
  
  /* Weighted sum of characters in "sumargv". */
  for (p = argv_me, i = 0; *p; ++p, ++i)
    sum += (*p - 'a') * i;
  
  /* Weighted sum of characters in `argv[0]'. */
  for (p = argv[0], i = 0; *p; ++p, ++i)
    sum -= (*p - 'a') * i;
  
  /* The `sum' should now be zero if the program name `argv[0]' is
   * correctly set up. */
  
  /* Interpret all remaining command line words as numbers and sum them. */
  for (i = 1; i < argc; i++)
    sum += atoi(argv[i]);
  
  /* If `argv' ends correctly with a null pointer this has no effect. */
  sum += (int)argv[argc];

  /* The final return value should now be the sum of the numbers
   * specified after sumargv on the command line. */
  return sum;
}
