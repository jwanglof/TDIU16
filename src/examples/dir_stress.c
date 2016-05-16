/* klaar@ida
   
   pintos -v -k -T 180 --fs-disk=2 --qemu -p ../examples/create_file -a create_file -p ../examples/create_remove_file -a cr_rm_file -p ../examples/dir_stress -a dir_stress -- -f -q run dir_stress
   
*/

#include <stdio.h>  
#include <syscall.h>

#define CREATORS  20   /* no of procs to concurrently create files */
#define LOADERS   11   /* no of procs to create/remove */
#define CYCLES   100   /* how many create/remove cycles to do */
#define TRIES     20   /* how many tries to create duplicates */
#define BUFSIZE   40   /* exec cmd line buffer */

int main(void)
{
  char buffer[BUFSIZE];
  int creator_pid[CREATORS];
  int loader_pid[LOADERS];
  int exit_status = 0;
  int i, n;

  /* put some load on the directory */
  for (i = 0; i < LOADERS; ++i)
  {
    snprintf(buffer, BUFSIZE, "cr_rm_file load.%d %d", i, CYCLES);
    loader_pid[i] = exec(buffer);
  }

  for (n = 0; n < TRIES; ++n)
  {
    int success_count = 0;
    int crash_count = 0;
    int fail_count = 0;
    
    /* set up processes to simultaneously create same file */
    for (i = 0; i < CREATORS; ++i)
    {
      snprintf(buffer, BUFSIZE, "create_file file.%d", n);
      creator_pid[i] = exec(buffer);
    }

    /* all creators will wait for this file to appear */
    create("go", 1);

    /* wait for creators to finish */
    for (i = 0; i < CREATORS; ++i)
    {
      int r = wait(creator_pid[i]);
      success_count += (r == 0);
      crash_count += (r == -1);
      fail_count += (r == 1);
    }
  
    remove("go");

    /* clean up the created file */
    snprintf(buffer, BUFSIZE, "file.%d", n);
    remove(buffer);

    /* only one creator should succed */
    if (success_count != 1)
    {
      printf("ERROR: %d files with same name created\n",
             success_count);
      exit_status = -1;
    }
    if (crash_count != 0)
    {
      printf("ERROR: %d creator processes crashed\n",
             crash_count);
      exit_status = -1;
    }
    if (fail_count != (CREATORS - 1))
    {
      printf("ERROR: %d creator processes failed\n",
             fail_count);
      exit_status = -1;
    }
  }
  
  for (i = 0; i < LOADERS; ++i)
  {
    int result = wait( loader_pid[i] );
    if ( result != 0 )
    {
      printf("ERROR: loader %d finished with code %d\n",
             loader_pid[i], result);
      exit_status = -1;
    }      
  }
  
  return exit_status;
}
