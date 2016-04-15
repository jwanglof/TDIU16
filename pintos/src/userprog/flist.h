#ifndef _MAP_H_
#define _MAP_H_

#include <lib/stdbool.h>
#include "filesys/file.h"

/* Place functions to handle a process open files here (file list).
   
   flist.h : Your function declarations and documentation.
   flist.c : Your implementation.

   The following is strongly recommended:

   - A function that given a file (struct file*, see filesys/file.h)
     and a process id INSERT this in a list of files. Return an
     integer that can be used to find the opened file later.

   - A function that given an integer (obtained from above function)
     and a process id FIND the file in a list. Should return NULL if
     the specified process did not insert the file or already removed
     it.

   - A function that given an integer (obtained from above function)
     and a process id REMOVE the file from a list. Should return NULL
     if the specified process did not insert the file or already
     removed it.
   
   - A function that given a process id REMOVE ALL files the specified
     process have in the list.

   All files obtained from filesys/filesys.c:filesys_open() are
   considered OPEN files and must be added to a list or else kept
   track of, to guarantee ALL open files are eventyally CLOSED
   (probably when removed from the list(s)).
 */

typedef struct file* value_t;
typedef int key_t;

//struct process_and_file {
//  int process_id;
//  struct file* file;
//};

/* symbolisk konstant för att lätt kunna ändra storleken i fortsättningen används denna då storleken behövs */
#define MAP_SIZE 128
#define START_POSITION 50
struct flist
{
  value_t content[MAP_SIZE];
  bool initiated;
};

bool flist_can_insert(struct flist *);

void flist_init(struct flist *);
int flist_get_next_free_position(struct flist *);
void flist_reset_position(struct flist *, int);
int flist_insert(struct flist *, struct file*);
struct file* flist_get_from_index(struct flist *, int);
struct file* flist_get_from_fd(struct flist *, int);
int flist_remove(struct flist *, int);
void flist_remove_all(struct flist *);


#endif /* userprog/flist.h */
