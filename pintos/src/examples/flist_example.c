/* flist_example.c

   Simple program to test whether running a user program works.

   Just invokes a system call that shuts down the OS. */
#include <syscall.h>
#include <lib/user/syscall.h>
#include "userprog/flist.h"

int main (void)
{
  struct flist new_flist;

  flist_init(&new_flist);

  return 0;
}
