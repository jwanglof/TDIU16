#define _POSIX_C_SOURCE 2
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
  YOUR WORK:

  You will complete the function setup_main_stack below. Missing code
  and calculations are expressed with a questionmark (?). The
  assignment is mostly about doing the correct math. To do that math
  you will need to learn or understand:

  - How the stack is used during program execution
  - How the stack is expected to look like at start of a C-program
  - What a pointer variable is, how it is used
  - How a C-array (pointer to many) is laid-out in memory
  - How pointer arithmetic behaves
  - How to cast from one pointer type to another
  - How to use printf to print good debug information
  - How to use a C-struct, both object and pointer to

  Some manual-pages of interest:

  man -s3 printf
  man -s3 strlen
  man -s3 strncpy
  man -s3 strtok_r

  The prototype for above functions:

  int printf(const char *restrict format, ...);
  size_t strlen(const char *s);
  size_t strncpy(char *dst, const char *src, size_t dstsize);
  char *strtok_r(char *s1, const char *s2, char **lasts);

  The above functions exist in Pintos code. Note the Pintos use
  'strlcpy' instead of 'strncpy'. If 'dst' is large enough they behave
  identical. You can find Pintos implementations in
  /home/TDIU16/labs/skel/pintos/src/lib, see string.c and stdio.c. It
  is strongly recommendable to look at the strtok_r examples given in
  string.c

  About *restrict: http://en.wikipedia.org/wiki/Pointer_alias


  Recommended compile command:

  gcc -m32 -Wall -Wextra -std=gnu99 -pedantic -g setup-argv.c

*/

#define true 1
#define false 0

typedef int bool;

/* "struct main_args" represent the stack as it must look when
 * entering main. The only issue: argv must point somewhere...
 *
 * The members of this structure is is directly related to the
 * arguments of main in a C-program, of course.
 *
 * int main(int argc, char* argv[]);
 *
 * (char** argv is a more generic form of char* argv[])
 *
 * The function pointer is normally handled by the compiler
 * automatically, and determine the address at where a function shall
 * continue when it returns. The main function does not use it, as the
 * operating system arrange for the program to stop execution when
 * main is finished.
 */
struct main_args
{
  /* Hint: When try to interpret C-declarations, read from right to
   * left! It is often easier to get the correct interpretation,
   * altough it does not always work. */

  /* Variable "ret" that stores address (*ret) to a function taking no
   * parameters (void) and returning nothing. */
  void (*ret)(void);

  /* Just a normal integer. */
  int argc;

  /* Variable "argv" that stores address to an address storing char.
   * That is: argv is a pointer to char*
   */
  char** argv;
};

/* A function that dumps 'size' bytes of memory starting at 'ptr'
 * it will dump the higher adress first letting the stack grow down.
 */
void dump(void* ptr, int size)
{
  int i;

  printf("Adress  \thex-data \tchar-data\n");

  for (i = size - 1; i >= 0; --i)
  {
    void** adr = (void**)((unsigned)ptr + i);
    unsigned char* byte = (unsigned char*)((unsigned)ptr + i);

    printf("%08x\t", (unsigned)ptr + i); /* address */

    if ((i % 4) == 0)
      /* seems we're actually forbidden to read unaligned adresses */
      printf("%08x\t", (unsigned)*adr); /* content interpreted as address */
    else
      printf("        \t"); /* fill */

    if(*byte >= 32 && *byte < 127)
      printf("%c\n", *byte); /* content interpreted as character */
    else
      printf("\\%o\n", *byte);

    if ((i % 4) == 0)
      printf("------------------------------------------------\n");
  }
}

/* Read one line of input ...
 */
void custom_getline(char buf[], int size)
{
  int i;
  for (i = 0; i < (size - 1); ++i)
  {
    buf[i] = getchar();
    if (buf[i] == '\n')
      break;
  }
  buf[i] = '\0';
}

/* Return true if 'c' is fount in the c-string 'd'
 * NOTE: 'd' must be a '\0'-terminated c-string
 */
bool exists_in(char c, const char* d)
{
  int i = 0;
  while (d[i] != '\0' && d[i] != c)
    ++i;
  return (d[i] == c);
}

/* Return the number of words in 'buf'. A word is defined as a
 * sequence of characters not containing any of the characters in
 * 'delimeters'.
 * NOTE: arguments must be '\0'-terminated c-strings
 */
int count_args(const char* buf, const char* delimeters)
{
  int i = 0;
  bool prev_was_delim;
  bool cur_is_delim = true;
  int argc = 0;

  while (buf[i] != '\0')
  {
    prev_was_delim = cur_is_delim;
    cur_is_delim = exists_in(buf[i], delimeters);
    argc += (prev_was_delim && !cur_is_delim);
    ++i;
  }
  return argc;
}

/* Replace calls to STACK_DEBUG with calls to printf. All such calls
 * easily removed later by replacing with nothing. */
#define STACK_DEBUG(...) printf(__VA_ARGS__)

/*void* setup_main_stack(const char* command_line, void* stack_top)
{
  *//* Variable "esp" stores an address, and at the memory loaction
   * pointed out by that address a "struct main_args" is found.
   * That is: "esp" is a pointer to "struct main_args" *//*
  struct main_args* esp;
  int argc;
  int total_size;
  int line_size;
  *//* "cmd_line_on_stack" and "ptr_save" are variables that each store
   * one address, and at that address (the first) char (of a possible
   * sequence) can be found. *//*
  char* cmd_line_on_stack;
  char* ptr_save;
  int i = 0;

  STACK_DEBUG("# command_line = %s\n", command_line);


  *//* calculate the bytes needed to store the command_line *//*
  line_size = strlen(command_line);
  STACK_DEBUG("# line_size = %d\n", line_size);

  *//* round up to make it even divisible by 4 *//*
  int alignment = line_size % 4;  // plus % 4
  STACK_DEBUG("# alignment = %d\n", alignment);
  if (alignment > 0) {
    alignment = 4 - alignment;
  }
  line_size += alignment;
  STACK_DEBUG("# line_size (aligned) = %d\n", line_size);

  *//* calculate how many words the command_line contain *//*
  argc = count_args(command_line, " ");
  STACK_DEBUG("# argc = %d\n", argc);

  *//* calculate the size needed on our simulated stack *//*
  total_size = line_size;  // argc-total-length-size
  total_size += argc * 4;  // argc-size in bytes
  total_size += 4;  // argv's
  total_size += 4;  // last argv
  total_size += 4;  // argc-address
  total_size += 4;  // return address
  STACK_DEBUG("# total_size = %d\n", total_size);

  *//* calculate where the final stack top will be located *//*
  esp = (struct main_args*) ((unsigned)stack_top - total_size);

  STACK_DEBUG("# esp: %p\n", (void*)esp);
  STACK_DEBUG("# stack_top: %p\n", stack_top);

  *//*
   *
   * setup return address and argument count
   *
   *//*

  *//* Variable "ret" that stores address (*ret) to a function taking no
   * parameters (void) and returning nothing. *//*
  *//**
   *  From the lab-info: Dock är det bra om returadressen är NULL ur felsäkerhetssynpunk
   *//*
//  esp->ret = esp - 4 ;  //??
  //void (*ret)(void);
  esp->ret = NULL;
  STACK_DEBUG("# esp->ret: %i\n", (int)esp->ret);

  *//* Just a normal integer. *//*
  //int argc;
  esp->argc = argc ;
  STACK_DEBUG("# esp->argc: %i\n", esp->argc);

  *//* calculate where in the memory the argv array starts *//*
  *//* Variable "argv" that stores address to an address storing char.
   * That is: argv is a pointer to char*
   *//*
  //char** argv;
//  esp->argv = (char **) cmd_line_on_stack;
  // + 12 since we need to take into the account that the return-address, argc, and char** argv is at the bottom of the stack
  esp->argv = (char **) ((unsigned)esp + 12);
  STACK_DEBUG("# esp->argv: %p\n", *esp->argv);

  *//* calculate where in the memory the words is stored *//*
  cmd_line_on_stack = (char *) ((unsigned)esp + (argc * 4) + 12 + 4);
  STACK_DEBUG("# cmd_line_on_stack: %p\n", cmd_line_on_stack);

  *//* copy the command_line to where it should be in the stack *//*
//  strncpy((char*)((unsigned) stack_top - line_size), command_line, strlen(command_line));
  strncpy((char*)((unsigned) stack_top - line_size), command_line, line_size);

  *//* build argv array and insert null-characters after each word *//*
  char *token;
//  for (token = strtok_r((char *) command_line, " ", &ptr_save); token != NULL; token = strtok_r(NULL, " ", &ptr_save)) {
  for (token = strtok_r(cmd_line_on_stack, " \n", &ptr_save); token != NULL; token = strtok_r(NULL, " \n", &ptr_save)) {
    STACK_DEBUG("# %i --- %s\n", i, token);
    esp->argv[i] = token;
    i++;
  }

  return esp; *//* the new stack top *//*
}*/

void* setup_main_stack(const char* command_line, void* stack_top)
{
  /* Variable "esp" stores an address, and at the memory loaction
   * pointed out by that address a "struct main_args" is found.
   * That is: "esp" is a pointer to "struct main_args" */
  struct main_args* esp;
  int argc;
  int total_size;
  int line_size;
  /* "cmd_line_on_stack" and "ptr_save" are variables that each store
   * one address, and at that address (the first) char (of a possible
   * sequence) can be found. */
  char* cmd_line_on_stack;
  char* ptr_save;
  int i = 0;


  /* calculate the bytes needed to store the command_line */
  // Add one for the last \0
  line_size = strlen(command_line) + 1;

  /* round up to make it even divisible by 4 */
  int alignment = line_size % 4;  // plus % 4
  if (alignment > 0) {
    alignment = 4 - alignment;
  }
  line_size += alignment;

  /* calculate how many words the command_line contain */
  argc = count_args(command_line, " ");

  /* calculate the size needed on our simulated stack */
//  total_size = line_size;  // argc-total-length-size
//  total_size += argc * 4;  // argc-size in bytes
//  total_size += 4;  // argv's
//  total_size += 4;  // last argv
//  total_size += 4;  // argc-address
//  total_size += 4;  // return address
  // sizeof(struct) = total size of the main_args struct
  // sizeof(char*) gives exact byte of a char* (4 byte)
  // (argc + 1) the amount of argc's plus program name
  total_size = line_size + sizeof(struct main_args) + sizeof(char*) * (argc + 1);

  /* calculate where the final stack top will be located */
  // Need to cast since stack_top is an address
  esp = (struct main_args*) ((unsigned)stack_top - total_size);

  /*
   *
   * setup return address and argument count
   *
   */

  /* Variable "ret" that stores address (*ret) to a function taking no
   * parameters (void) and returning nothing. */
  /**
   *  From the lab-info: Dock är det bra om returadressen är NULL ur felsäkerhetssynpunk
   */
//  esp->ret = esp - 4 ;  //??
  //void (*ret)(void);
  esp->ret = NULL;

  /* Just a normal integer. */
  //int argc;
  esp->argc = argc ;

  /* calculate where in the memory the argv array starts */
  /* Variable "argv" that stores address to an address storing char.
   * That is: argv is a pointer to char*
   */
  //char** argv;
//  esp->argv = (char **) cmd_line_on_stack;
  // + 12 since we need to take into the account that the return-address, argc, and char** argv is at the bottom of the stack
//  esp->argv = (char **) ((unsigned)esp + 12);
  esp->argv = (char **) ((unsigned)esp + sizeof(struct main_args));

  /* calculate where in the memory the words is stored */
//  cmd_line_on_stack = (char *) ((unsigned)esp + (argc * 4) + 12 + 4);
  cmd_line_on_stack = (char *) ((unsigned)stack_top - line_size);

//  /* copy the command_line to where it should be in the stack */
////  strncpy((char*)((unsigned) stack_top - line_size), command_line, strlen(command_line));
//  strlcpy((char*)((unsigned) stack_top - line_size), command_line, line_size);
//
//  /* build argv array and insert null-characters after each word */
//  char *token = 0;
//  for (token = strtok_r((char *) command_line, " ", &ptr_save); token != NULL; token = strtok_r(NULL, " ", &ptr_save)) {
////  for (token = strtok_r(cmd_line_on_stack, " \n", &ptr_save); token != NULL; token = strtok_r(NULL, " \n", &ptr_save)) {
//    debug("# %i --- %s\n", i, token);
//    esp->argv[i] = token;
//    i++;
//  }

  char* token = 0;
  for(token = strtok_r ( (char*)command_line, " ", &ptr_save); token != NULL;
      token = strtok_r (NULL, " ", &ptr_save))
  {
    int token_size = strlen(token) + 1;
    strncpy(cmd_line_on_stack, token, token_size);

    esp->argv[i] = cmd_line_on_stack;
    cmd_line_on_stack += token_size;
    i++;
  }

  return esp; /* the new stack top */
}

/* The C way to do constants ... */
#define LINE_SIZE 1024

int main()
{
  struct main_args* esp;
  char line[LINE_SIZE];
  void* simulated_stack = malloc(4096);
  void* simulated_stack_top = (void*)((unsigned)simulated_stack + 4096);
  int i;

  /* read one line of input, this will be the command-line */
  printf("Mata in en mening: ");
  custom_getline(line, LINE_SIZE);

  /* put initial content on our simulated stack */
  esp = setup_main_stack(line, simulated_stack_top);
  printf("# esp = %08x\n", (unsigned)esp);

  /* dump memory area for verification */
  dump(esp, (unsigned)simulated_stack_top - (unsigned)esp);

  /* original command-line should not be needed anymore */
  for (i = 0; i < LINE_SIZE; ++i)
    line[i] = (char)0xCC;

  /* print the argument vector to see if it worked */
  for (i = 0; i < esp->argc; ++i)
  {
    printf("argv[%d] = %s\n", i, esp->argv[i]);
  }
  printf("argv[%d] = %p\n", i, esp->argv[i]);

  free(simulated_stack);

  return 0;
}
