/* klaar@ida

   This program prints some ascii art...
 */
#include <syscall.h>
#include <stdio.h>
#include <string.h>

int
main (void)
{
  const char* msg[5] = {
    " Y   Y  OOO  O   O | RRR  EEEE    DDD    OOO  N   N EEEE \n",
    " Y   Y O   O O   O | R  R E       D  D  O   O NN  N E    \n",
    "  YYY  O   O O   O   RRR  EEE     D   D O   O N N N EEE  \n",
    "   Y   O   O O   O   R  R E       D  D  O   O N  NN E    \n",
    "   Y    OOO   OOO    R  R EEEE    DDD    OOO  N   N EEEE \n"
  };
  int i;

  for (i = 0; i < 5; ++i)
  {
    write (STDOUT_FILENO, msg[i], strlen(msg[i]));
  }
  
  return 0;
}
