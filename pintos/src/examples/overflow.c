/* klaar@ida
  
   noopsled | pintos --fs-disk=2 -v -k -p ../examples/overflow -a overflow -p ../examples/crack -a crack -- -f -q run overflow
  
   This program is possible to crack with carefully crafted input.
   It examplifies the danger of buffer overflow.
*/

#include <syscall.h>
#include <stdio.h>
#include <string.h>

static void stringcopy(char* dst, const char* src)
{
  while (*src)
    *dst++ = *src++;
  *dst = '\0';
}

int main(void);

/* A messy not very good buffer overflow example. A little bit too
 * contrieved. */
static int getline (char* destination)
{
  char line[200];
  int i = 0;
  char* dst = destination;

//#define DEBUG_CODE
#ifdef DEBUG_CODE
  int r, c;
  unsigned* ret = (unsigned*)(&dst - 1);

  printf ("Return address address: 0x%08x\n", (unsigned)&ret);
  printf ("Return address content: 0x%08x\n", *ret);
  printf ("Main function address : 0x%08x\n", (unsigned)main);
  printf ("Line buffer address   : 0x%08x\n", (unsigned)line);
#endif
  
  do /* !!! Buffer overflow when i >= 200 !!! */
  {
    if ( read (STDIN_FILENO, &line[i], 1) != 1)
      break; /* failed to read requested number of characters */
  }
  while ( line[i++] != '\n' );
  
  line[i-1] = '\0';
  
#ifdef DEBUG_CODE
  /* hex dump of read data */
  for (r = 0; r < 16; ++r)
  {
    printf ("0x%08x: ", (unsigned)&line[ 16*r ]);
    for (c = 0; c < 16; ++c)
    {
      int code = line[ 16*r + c ] & 0xff;
      printf("\\x%02x", code);
    }
    printf("\n");
  }
        
  printf ("Return address content: 0x%08x\n", *ret);
#endif

  stringcopy(dst, line);

  return ( strlen(line) > 1 );
}

/* Stupid program to echo every line you write to screen. And to make
 * matter worse, getline have a serious buffer overflow. */
int main (void)
{
  char msg[2000];
  char quote = '"';
  char endl = '\n';
  
  while ( getline (msg) )
  {
    write (STDOUT_FILENO, &quote, 1);
    write (STDOUT_FILENO, msg, strlen(msg));
    write (STDOUT_FILENO, &quote, 1);
    write (STDOUT_FILENO, &endl, 1);
  }
  
  return 0;
}
