/* klaar@ida

   pintos --fs-disk=2 -v -k -p ../examples/line_echo -a line_echo -- -f -q run line_echo

   This program will echo every input line to output.
 */

#include <stdio.h>
#include <syscall.h>

int mystrlen(char *start);
int getline(char *buf, int size);

int main(void)
{
  char buf[10];
  int length;
  char endl = '\n';

  for ( ; ; )
  {
    length = getline(buf, 10);
    
    if (length != mystrlen(buf))
      exit(111);

    if (length < 1)
      break;
    
    write(STDOUT_FILENO, buf, length);
    write(STDOUT_FILENO, &endl, 1);
  }
  
  return 0;
}

int getline(char *buf, int size)
{
  int i;
  
  for (i = 0; i < size-1; ++i)
  {
    if (read(STDIN_FILENO, &buf[i], 1) != 1)
      break;
    if (buf[i] == '\n')
      break;
  }
  buf[i] = '\0';
  return i;
}

int mystrlen(char *start)
{
  char* end = start;
  
  while(*end != '\0')
    ++end;
  
  return (end - start);
}
