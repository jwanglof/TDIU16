/*
  Try to load a corrupt executable.
  (klaar@ida)
*/

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  msg ("exec(\"corrupt-elf\"): %d", exec ("corrupt-elf"));
}
