#include <stdio.h>

typedef __SIZE_TYPE__ size_t;

/* As strlcpy, but only copies the first word from SRC. Worde are
 * delimeted by space. Leading space is also ignored.
 * ( added by klaar@ida )
 */
size_t
strlcpy_first_word (char *dst, const char *src, size_t size) 
{
  size_t i = size;
  
  /* skip leading spaces */
  while (*src != '\0' && *src == ' ')
    ++src;

  /* copy at most size characters */
  if (size > 0)
  {
    while (i > 1)
    {
      if (*src == '\0' || *src == ' ')
        break;
      
      *dst++ = *src++;
      --i;
    }
    *dst = '\0';
  }
  return size - i;
}

int main(int argc, char* argv[])
{
  char word[100];
  int i;
  
  i = strlcpy_first_word(word, argv[1], -1);

  printf("Argv[1]: '%s', (%d characters)\n", argv[1], strlen(argv[1]));
  printf("First w: '%s', (%d characters)\n", word, strlen(word));
  printf("Copied %d characters of max %d\n\n", i, -1);

  
  i = strlcpy_first_word(word, argv[1], 0);

  printf("Argv[1]: '%s', (%d characters)\n", argv[1], strlen(argv[1]));
  printf("First w: '%s', (%d characters)\n", word, strlen(word));
  printf("Copied %d characters of max %d\n\n", i, 0);


  i = strlcpy_first_word(word, argv[1], 1);

  printf("Argv[1]: '%s', (%d characters)\n", argv[1], strlen(argv[1]));
  printf("First w: '%s', (%d characters)\n", word, strlen(word));
  printf("Copied %d characters of max %d\n\n", i, 1);


  i = strlcpy_first_word(word, argv[1], 10);

  printf("Argv[1]: '%s', (%d characters)\n", argv[1], strlen(argv[1]));
  printf("First w: '%s', (%d characters)\n", word, strlen(word));
  printf("Copied %d characters of max %d\n\n", i, 10);

}
