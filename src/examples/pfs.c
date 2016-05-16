/* klaar@ida (cleanup of previous code by various IDA-employees)
   
   pintos -v -k -T 120 --fs-disk=2 --qemu -p ../examples/pfs -a pfs -p ../examples/pfs_writer -a pfs_writer -p ../examples/pfs_reader -a pfs_reader -g messages -- -F=20000 -f -q run pfs
   
   Tests that read and write are properly synchronized. Also stresses
   the filesystem somewhat.

   pfs_reader and pfs_writer are needed child processes.
*/
  
#include "syscall.h"

int main(void)
{
  int i;
  int pid[5];
  
  create("file.1", 50000);
  create("messages", 5000);
  
  pid[0] = exec("pfs_writer a z");
  pid[1] = exec("pfs_writer A Z");
  pid[2] = exec("pfs_reader");
  pid[3] = exec("pfs_reader");
  pid[4] = exec("pfs_reader");
  
  for (i = 0; i < 5; i++)
  {
    wait(pid[i]);
  }
  exit(0);
}
