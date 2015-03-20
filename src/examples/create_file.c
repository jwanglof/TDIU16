/* klaar@ida
*/
  
#include <stdio.h>
#include <syscall.h>

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    printf("%s: bad arguments\n", argv[0]);
    return 1;
  }
  
  while ( open("go") == -1 )
    ;
  
  if ( ! create(argv[1], 500) )
    return 1;
  
  return 0;
}
