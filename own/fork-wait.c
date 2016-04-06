#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* The printf text serve as further comments. Read it. */

int main(int argc, char* argv[])
{
  pid_t pid;
  char dummy[16];

  if (argc < 2)
  {
    printf("You need to specify a program as argument 1.\n");
    exit(1); /* Terminate the process with error (1). */
  }

  printf("P | Switch to a differen terminal.\n"
         "P | Execute the command \"ptree $USER\".\n"
         "P | It will show a list of processes you are running.\n"
         "P | You should see the process '%s' once.\n"
         "P | Press enter when you verified that.\n\n", argv[0]);

  /* Wait until enter is pressed (gives you time...) */
  fgets(dummy, 16, stdin);

  printf("P | This process will now clone itself with fork.\n"
         "P | One (almost) identical copy will be started.\n"
         "P | The only difference is the result of fork()\n\n");

  pid = fork();

  if (pid == 0)
  {
    printf("  C | This is printed by the new process (the CHILD)\n"
           "  C | The child get a 0 return value from fork.\n\n");

    printf("  C | Repeat the previous command in the other terminal.\n"
           "  C | You should see the process '%s' twice now.\n"
           "  C | (grep for it if you run many processes)\n"
           "  C | Press enter when you verified that.\n\n", argv[0]);

    /* Wait until enter is pressed (gives you time...) */
    fgets(dummy, 16, stdin);

    printf("  C | The program specified as first argument (%s) on the \n"
           "  C | command line will now be started by the child process.\n"
           "  C | This will REPLACE the code of the child with the code\n"
           "  C | of the specified program.\n\n", argv[1]);

    /* specify a terminal as argument 1 and check ptree again ...
     * you will see that the child is replaced, it have the same
     * process number, but a new command line
     */
    execv(argv[1], argv + 1);

    printf("  C | This will never be printed, since after starting the new\n"
           "  C | program by calling exec this old program is replaced by\n"
           "  C | the new. This is only printed if the new program could not\n"
           "  C | be started for some reason.\n\n");

    exit(-1); /* Terminate the child with error (-1). */
  }
  else
  {
    int ret = 0; /* Used to save the result of the child. */

    printf("P | This is printed by the origingal process (the PARENT)\n"
           "P | It got the number (%d) from fork to identify the child.\n"
           "P | This process will now work for 5 seconds.\n\n", (int)pid);
    sleep(5); /* pretend */

    printf("P | This is printed by the origingal process (the PARENT)\n"
           "P | It got the number (%d) from fork to identify the child.\n"
           "P | It is now used to wait for the result of that child\n\n",
           (int)pid);

    /* Wait for the child to finish and get it's status in ret. */
    waitpid(pid, &ret, 0);

    /* Print the exit status of the child. */
    if (WIFEXITED(ret))
    {
      printf("P | Child finished with code %d\n\n", WEXITSTATUS(ret));
    }
    else if (WIFSIGNALED(ret))
    {
      printf("P | Child finished due to signal %d\n\n", WTERMSIG(ret));
    }
  }

  return 0;
}
