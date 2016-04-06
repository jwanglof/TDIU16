/*
  Simulate a failure in thread_create.
  A real reason for failure could be low memory.
  (klaar@ida)
*/

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  msg ("exec(\"child-simple\"): %d", exec ("child-simple"));
}
