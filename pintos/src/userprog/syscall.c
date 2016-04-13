#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

/* header files you probably need, they are not used yet */
#include <string.h>
#include <lib/kernel/stdio.h>
#include <lib/user/syscall.h>
#include <filesys/inode.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"

#define DBG(sys_call, format, ...) printf("# DEBUG: " sys_call " -- " format "\n", ##__VA_ARGS__)
//#define DBG(format, ...)

static void syscall_handler (struct intr_frame *);
struct flist opened_files;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  // Initiate the open-file list
  flist_init(&opened_files);
}


/* This array defined the number of arguments each syscall expects.
   For example, if you want to find out the number of arguments for
   the read system call you shall write:
   
   int sys_read_arg_count = argc[ SYS_READ ];
   
   All system calls have a name such as SYS_READ defined as an enum
   type, see `lib/syscall-nr.h'. Use them instead of numbers.
 */
const int argc[] = {
  /* basic calls */
  0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1, 
  /* not implemented */
  2, 1,    1, 1, 2, 1, 1,
  /* extended */
  0
};


static void
syscall_handler (struct intr_frame *f) 
{
  int32_t* esp = (int32_t*)f->esp;
  uint32_t eax = (uint32_t)f->eax;

//  DBG("Stack top + 0: %d", esp[0]);
//  DBG("Stack top + 1: %d\n", esp[1]);
//  DBG("TOP", "EAX: %i", eax);


  switch ( esp[0] /* retrive syscall number */ )
  {

    case SYS_HALT:
    {
//      DBG("SYS_HALT", "Sys-call number: %i. ESP: %i", SYS_HALT, esp[0]);
      power_off();
    }

    case SYS_EXIT:
    {
      /**
       * Frågor och svar från uppgiften:
       * 1) Vilka funktioner är involverade i att avsluta en tråd? En process?
       *    Svar: En tråd: thread_exit()
       *          En process: process_exit() och process_cleanup()
       * 2) Vilken uppgift har respektive funktion?
       *    Svar: thread_exit() sätter en terminate-flagga på sigsjälv och byter till nästa tråd
       *          process_exit() ska ta emot en exit status och avsluta processen
       *          process_cleanup() avallokerar alla resurser en process använt
       * 3) I vilken ordning exekveras de?
       *    Svar: thread_exit(), process_cleanup(), process_exit()
       * 4) I vilken fil är parametrarna till varje systemanrop specificerad?
       *    Svar: För trådar:
       *          För processer: userprog/process.c
       * 5) Var i Pintos minne kan du hitta parametrarna?
       *    Svar: I minnesutrymmet som startar på adressen PHYS_BASE
       * 6) Vad skall operativsystemet göra med parametern till exit (exit status)?
       *    Svar: ????
       * 7) Vad händer när main returnerar?
       *    Svar: Processorna fortsätter exekvera från återhoppsadressen och stackpekaren räknas upp för att komma tillbaka till ursprungsläget
       */
//      DBG("SYS_EXIT", "Sys-call number: %i. ESP: %i", SYS_EXIT, esp[0]);
//      DBG("SYS_EXIT", "Status: %i", esp[1]);
      thread_exit();
//      process_exit(esp[1]);
    }

    case SYS_READ:
    {
      // esp[#]
      // 1 = int fd
      // 2 = void *buffer
      // 3 = unsigned length
      int fd = esp[1];
      char *buffer = (char *) esp[2];
      unsigned length = (unsigned int) esp[3];

      DBG("SYS_READ", "Sys-call number: %i. ESP: %i", SYS_READ, esp[0]);
      DBG("SYS_READ", "FD: %i", fd);
      DBG("SYS_READ", "Length: %i", length);

      if (fd == STDIN_FILENO) {
//        DBG("SYS_READ", "Reading from keyboard!");
        unsigned int position = 0;
        while (position < length) {
          char c = input_getc();
//          DBG("SYS_READ", "Char: %c -- Position: %i", c, position);
          // Replace \r with \n
          if (c == '\r') {
            c = '\n';
          }
          buffer[position] = c;
          putbuf(&c, 1);
          position++;
        }
        f->eax = length;
//        DBG("SYS_READ", "DONE reading from keyboard!");
      } else if (fd == STDOUT_FILENO) {
        DBG("SYS_READ", "PANIC!!");
        // Eh?
        f->eax = (uint32_t) -1;
      } else {
        DBG("SYS_READ", "Is the file opened?");

        struct file* opened_file = flist_get_from_index(&opened_files, thread_current()->tid, fd);
        if (opened_file != NULL) {
          DBG("SYS_READ", "File is opened");
          off_t bytes_read = file_read(opened_file, buffer, length);
          f->eax = (uint32_t) bytes_read;
        } else {
          DBG("SYS_READ", "File not opened!");
          f->eax = (uint32_t) -1;
        }
      }
      break;
    }

    case SYS_WRITE:
    {
      // esp[#]
      // 1 = int fd
      // 2 = const void *buffer
      // 3 = unsigned length
      int fd = esp[1];
      char *buffer = (char *) esp[2];
      unsigned length = (unsigned int) esp[3];

//      DBG("SYS_WRITE", "Sys-call number: %i. ESP: %i", SYS_WRITE, esp[0]);
//      DBG("SYS_WRITE", "FD: %i", fd);
      DBG("SYS_WRITE", "Length: %i", length);

      if (fd == STDIN_FILENO) {
        DBG("SYS_WRITE", "PANIC!!!");
        // Eh?
        f->eax = (uint32_t)-1;
      } else if (fd == STDOUT_FILENO) {
        DBG("SYS_WRITE", "Writing to monitor! Length: %i", length);
        putbuf(buffer, length);
        printf("\n");
        DBG("SYS_WRITE", "DONE writing to monitor!");
        f->eax = (uint32_t) length;
      } else {
        // Check if FD is in the opened-file-list
        DBG("SYS_WRITE", "ELSE");
        struct file* opened_file = flist_get_from_index(&opened_files, thread_current()->tid, fd);
        if (opened_file != NULL) {
          off_t bytes_write = file_write(opened_file, buffer, length);
          f->eax = (uint32_t) bytes_write;
        } else {
          f->eax = (uint32_t) -1;
        }
      }

      break;
    }

    case SYS_CREATE:
    {
      // esp[#]
      // 1 = const char *file
      // 2 = unsigned initial_size
      char* filename = (char *) esp[1];
      int initial_size = esp[2];

      DBG("SYS_CREATE", "Sys-call number: %i. ESP: %i", SYS_OPEN, esp[0]);
      DBG("SYS_CREATE", "File: %s", filename);
      DBG("SYS_CREATE", "Initial size: %i", initial_size);
      bool file_created = filesys_create(filename, initial_size);
      DBG("SYS_CREATE", "File created: %i", file_created);
      // Return if the file was created or not
      f->eax = (uint32_t) file_created;
      break;
    }

    case SYS_OPEN:
    {
      // esp[#]
      // 1 = const char *file
      char* filename = (char *) esp[1];

      DBG("SYS_OPEN", "Sys-call number: %i. ESP: %i", SYS_OPEN, esp[0]);
      DBG("SYS_OPEN", "File: %s", filename);

      struct file *opened_file = filesys_open(filename);
      if (opened_file != NULL) {
        DBG("SYS_OPEN", "File exist!");
//        struct inode *opened_file_inode = file_get_inode(opened_file);

        int fd = flist_insert(&opened_files, thread_current()->tid, opened_file);
//        flist_get_from_index(&opened_files, thread_current()->tid, fd);

//        DBG("SYS_OPEN", "%s", opened_file_inode);
        f->eax = (uint32_t) fd;
      } else {
        DBG("SYS_OPEN", "File does not exist!");
        f->eax = (uint32_t) -1;
      }

      break;
    }

    case SYS_CLOSE:
    {
      // esp[#]
      // 1 = int fd
      int fd = esp[1];

      DBG("SYS_CLOSE", "Sys-call number: %i. ESP: %i", SYS_CLOSE, esp[0]);
      DBG("SYS_CLOSE", "FD: %i", fd);

      struct file* opened_file = flist_get_from_index(&opened_files, thread_current()->tid, fd);
      if (opened_file != NULL) {
        int removed_fd = flist_remove(&opened_files, thread_current()->tid, fd);
        if (removed_fd != NULL) {
          file_close(opened_file);
        }
      } else {
        DBG("SYS_CLOSE", "File is not opened!");
        // Eh?
        f->eax = (uint32_t) -1;
      }

      break;
    }

    case SYS_REMOVE:
    {
      // esp[#]
      // 1 = const char *file
      char* filename = (char *) esp[1];

      DBG("SYS_REMOVE", "Sys-call number: %i. ESP: %i", SYS_REMOVE, esp[0]);
      DBG("SYS_REMOVE", "Filename: %s", filename);

      bool file_removed = filesys_remove(filename);
      if (file_removed) {
        DBG("SYS_REMOVE", "File removed!");
      } else {
        DBG("SYS_REMOVE", "File could not be removed!");
      }
      f->eax = (uint32_t) file_removed;
      break;
    }

    case SYS_SEEK:
    {
      // esp[#]
      // 1 = int fd
      // 2 = unsigned position
      int fd = esp[1];
      unsigned position = (unsigned int) esp[2];

      struct file* opened_file = flist_get_from_fd(&opened_files, fd);
      if (opened_file != NULL) {
        file_seek(opened_file, position);
        f->eax = (uint32_t) fd;
      } else {
        f->eax = (uint32_t) -1;
      }

      break;
    }

    case SYS_TELL:
    {
      // esp[#]
      // 1 = int fd
      int fd = esp[1];

      struct file* opened_file = flist_get_from_fd(&opened_files, fd);
      if (opened_file != NULL) {
        off_t current_position = file_tell(opened_file);
        f->eax = (uint32_t) current_position;
      } else {
        f->eax = (uint32_t) -1;
      }

      break;
    }

    case SYS_FILESIZE:
    {
      // esp[#]
      // 1 = int fd
      int fd = esp[1];

      struct file* opened_file = flist_get_from_fd(&opened_files, fd);
      if (opened_file != NULL) {
        off_t filesize = file_length(opened_file);
        f->eax = (uint32_t) filesize;
      } else {
        f->eax = (uint32_t) -1;
      };

      break;
    }

    default:
    {
      printf ("Executed an unknown system call!\n");
      
      printf ("Stack top + 0: %d\n", esp[0]);
      printf ("Stack top + 1: %d\n", esp[1]);
      
      thread_exit ();
    }
  }
}
