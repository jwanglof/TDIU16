/* klaar@ida

   Create a buffer that spans invalid pages. Used to check memory
   verification.
 */
#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

/* global variables are placed by the compiler in the process image,
 * among the initialized variables
 * the process image is loaded at the bottom of the virtual memory space
 */
char global = 0;

int main (int argc, char *argv[])
{
   /* local variables are allocated on the proces stack during runtime
    * the stack is at top of the virtual memory space
    */
   char local = 0;
   int fd;
   int count;
   unsigned l, g;

   /* both adresses will of course be valid */
   printf("global: %p\n", &global);
   printf("local : %p\n", &local);

   /* and a pointer starting at the global variable adress, and ending
    * at the local variable addres will have valid start and end
    * but will all addresses (about 3 GB) inbetween be valid?
    */
   l = (unsigned)&local;
   g = (unsigned)&global;
   printf("size : %u MiB\n", (l - g)/1024/1024);

   create("somefile", 1000);
   fd = open("somefile");

   /* this tries to use the buffer (it should fail verification) */
   count = read(fd, &global, (l - g + 1));
   printf("read %i out of %u bytes\n", count, (l - g + 1));
   
   /* this tries to use a buffer probably ending before the stack */
   count = read(fd, &global, (l - g + 1) - 4096);
   printf("read %i out of %u bytes\n", count, (l - g + 1) - 4096);
   
   close(fd);

   return 0;
}
