/* halt.c

   Simple program to test whether running a user program works.
 	
   Just invokes a system call that shuts down the OS. */

#include <syscall.h>
#include <lib/user/syscall.h>

int
main (void)
{
  exit(123123);
  exit(888888);
  halt ();
  /* not reached */
}
