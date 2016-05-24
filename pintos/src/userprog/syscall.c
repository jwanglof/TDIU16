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
#include <devices/timer.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"
#include "plist.h"

#define DBG(sys_call, format, ...) printf("# DEBUG: " sys_call " -- " format "\n", ##__VA_ARGS__)
//#define DBG(format, ...)
#define DBG_T(sys_call, thread_id, format, ...) printf("# DEBUG: " sys_call "#%i -- " format "\n", thread_id, ##__VA_ARGS__)
#define DBG_CURR(sys_call, format, ...) printf("# DEBUG CURRENT: " sys_call " -- " format "\n", ##__VA_ARGS__)

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
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

  // TODO Måste verifiera att alla esp-index har giltiga adress


  switch ( esp[0] /* retrive syscall number */ )
  {

    case SYS_HALT:
    {
      DBG("SYS_HALT", "Sys-call number: %i. ESP: %i", SYS_HALT, esp[0]);
      power_off();
    }

    case SYS_EXIT:
    {
      // esp[#]
      // 1 = int status
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
      int status = esp[1];
      int tid = thread_current()->tid;

      DBG_T("SYS_EXIT", tid, "Sys-call number: %i. ESP: %i", SYS_EXIT, esp[0]);
      DBG_T("SYS_EXIT", tid, "Status: %i", esp[1]);
      process_exit(status);
      break;
    }

    case SYS_READ:
    {
      // esp[#]
      // 1 = int fd
      // 2 = void *buffer
      // 3 = unsigned length
      int fd = esp[1];
      char *buffer = (char *) esp[2];
      unsigned length = (unsigned) esp[3];

      int tid = thread_current()->tid;

//      DBG("SYS_READ", "Sys-call number: %i. ESP: %i", SYS_READ, esp[0]);
      DBG_T("SYS_READ", tid, "FD: %i", fd);
      DBG_T("SYS_READ", tid, "FD: %i. Length: %i", fd, length);

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
          putbuf(&c, 1);  // TODO putchar instead?
          position++;
        }
        f->eax = length;
//        DBG("SYS_READ", "DONE reading from keyboard!");
      } else if (fd == STDOUT_FILENO) {
        DBG_T("SYS_READ", tid, "PANIC!!");
        // Eh?
        f->eax = (uint32_t) -1;
      } else {
        DBG_T("SYS_READ", tid, "FD: %i. Is the file opened?", fd);

        struct file* opened_file = flist_get_from_index(&thread_current()->flist, fd);
        if (opened_file != NULL) {
          DBG_T("SYS_READ", tid, "FD: %i. File is opened", fd);
          off_t bytes_read = file_read(opened_file, buffer, length);
          f->eax = (uint32_t) bytes_read;
        } else {
          DBG_T("SYS_READ", tid, "FD: %i. File not opened!", fd);
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

      int tid = thread_current()->tid;

//      DBG("SYS_WRITE", "Sys-call number: %i. ESP: %i", SYS_WRITE, esp[0]);
      DBG_T("SYS_WRITE", tid, "#%i FD: %i", fd, fd);
      DBG_T("SYS_WRITE", tid, "#%i Length: %i", fd, length);

      if (fd == STDIN_FILENO) {
        DBG_T("SYS_WRITE", tid, "PANIC!!!");
        // Eh?
        f->eax = (uint32_t) -1;
      } else if (fd == STDOUT_FILENO) {
        DBG("SYS_WRITE", "Writing to monitor! Length: %i", length);
        putbuf(buffer, length);
        DBG("SYS_WRITE", "DONE writing to monitor!");
        f->eax = (uint32_t) length;
      } else {
        // Check if FD is in the opened-file-list
        DBG_T("SYS_WRITE", tid, "#%i ELSE", fd);
        struct file* opened_file = flist_get_from_index(&thread_current()->flist, fd);
        if (opened_file != NULL) {
          off_t bytes_write = file_write(opened_file, buffer, length);
          DBG_T("SYS_WRITE", tid, "#%i Length: %i, bytes write: %i", fd, length, bytes_write);
          if (length != (unsigned) bytes_write) {
            DBG_T("SYS_WRITE", tid, "ERROR #%i Didn't write everything!", fd);
          }
          f->eax = (uint32_t) bytes_write;
        } else {
          DBG_T("SYS_WRITE", tid, "#%i ERROR: opened_file is NULL: %i", fd, opened_file == NULL);
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
      int tid = thread_current()->tid;

      char* filename = (char *) esp[1];

//      DBG("SYS_OPEN", "Sys-call number: %i. ESP: %i", SYS_OPEN, esp[0]);
      DBG_T("SYS_OPEN", tid, "File: %s", filename);

      struct file *opened_file = filesys_open(filename);
      if (opened_file != NULL) {
        DBG_T("SYS_OPEN", tid, "File exist!");
        int fd = flist_insert(&thread_current()->flist, opened_file);
        // If something went wrong when inserting the file to flist we must close it
        if (fd == -1) {
          file_close(opened_file);
          f->eax = (uint32_t) -1;
        } else {
          f->eax = (uint32_t) fd;
        }
        DBG_T("SYS_OPEN", tid, "FD: %i", fd);
      } else {
        DBG_T("SYS_OPEN", tid, "File does not exist!");
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

      struct file* opened_file = flist_get_from_index(&thread_current()->flist, fd);
      if (opened_file != NULL && fd != -1) {
        // file_close is called in flist_remove()
        flist_remove(&thread_current()->flist, fd);
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

      struct file* opened_file = flist_get_from_fd(&thread_current()->flist, fd);
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

      struct file* opened_file = flist_get_from_fd(&thread_current()->flist, fd);
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

      struct file* opened_file = flist_get_from_fd(&thread_current()->flist, fd);
      if (opened_file != NULL) {
        off_t filesize = file_length(opened_file);
        f->eax = (uint32_t) filesize;
      } else {
        f->eax = (uint32_t) -1;
      };

      break;
    }

    case SYS_SLEEP:
    {
      // esp[#]
      // 1 = int millis
      int64_t millis = (int64_t) esp[1];

      DBG("SYS_SLEEP", "Sys-call number: %i. ESP: %i", SYS_SLEEP, esp[0]);
      DBG("SYS_SLEEP", "Millis: %d", (int) millis);

      timer_msleep(millis);

      break;
    }

    case SYS_PLIST:
    {
      // esp[#]

      DBG("SYS_PLIST", "Sys-call number: %i. ESP: %i", SYS_PLIST, esp[0]);

      process_print_list();

      break;
    }

    case SYS_EXEC:
    {
      // esp[#]
      // 1 = const char *file (aka command_line)
      const char *file = (const char*) esp[1];

      DBG("SYS_EXEC", "Sys-call number: %i. ESP: %i", SYS_EXEC, esp[0]);
      DBG("SYS_EXEC", "File: %s", file);

      int process_id = process_execute(file);
      DBG("SYS_EXEC", "Process ID: %i", process_id);
      f->eax = (uint32_t) process_id;

      break;
    }

    case SYS_WAIT:
    {
      // esp[#]
      // 1 = pid_t id
      pid_t child_id = esp[1];

      DBG("SYS_WAIT", "Sys-call number: %i. ESP: %i", SYS_WAIT, esp[0]);
      DBG("SYS_WAIT", "Child ID: %d", child_id);

      int status = process_wait(child_id);

      DBG("SYS_WAIT", "Status: %d", status);

      f->eax = (uint32_t) status;
      break;
    }

    default:
    {
      printf ("Executed an unknown system call!\n");
      
      printf ("Stack top + 0: %d\n", esp[0]);
      printf ("Stack top + 1: %d\n", esp[1]);
      printf ("Stack top + 2: %d\n", esp[2]);
      printf ("Stack top + 3: %d\n", esp[3]);

      thread_exit ();
    }
  }
}
