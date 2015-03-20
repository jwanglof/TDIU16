/* klaar@ida
*/
  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char* argv[])
{
  char buf[40];
  char ref[40];
  int loops;
  int i, n;
  int fd;
  
  if (argc != 3)
  {
    printf("%s: bad arguments\n", argv[0]);
    return 1;
  }
  
  loops = atoi(argv[2]);
  
  for (i = 0; i < loops; ++i)
  {
    if ( create(argv[1], 500) )
    {
      snprintf(ref, 40, "file '%s' iteration %d", argv[1], i);
      fd = open(argv[1]);
      remove(argv[1]);
      
      write(fd, ref, strlen(ref));
      seek(fd, 0);
      for (n = 0; n < 100; ++n)
        ;
      read(fd, buf, 40);      
      close(fd);

      if (strcmp(ref, buf) != 0)
      {
        printf("%s: ERROR bad file content\n", argv[0]);
        printf("%s: written  : '%s'\n", argv[0], ref);
        printf("%s: read back: '%s'\n", argv[0], buf);
        exit(1);
      }
      
      if ( (i * 10000 / loops) % 1000 == 0 )
        printf("%s: %d%% done\n", argv[0], (i * 100 / loops));
    }
    else
    {
      printf("%s: ERROR create '%s' failed\n", argv[0], argv[1]);
      exit(1);
    }
  }
  return 0;
}
