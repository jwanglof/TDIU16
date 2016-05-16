#include <stddef.h>
#include <stdio.h>

#include "flist.h"

//#define DBG(format, ...) printf("# FLIST - DEBUG: " format "\n", ##__VA_ARGS__)
#define DBG(format, ...)

/**
 * Returns true if the list accepts new files
 */
bool flist_can_insert(struct flist *flist) {
  return flist->initiated;
}

/**
 * Initiates the list
 * Resets all the entries so it's easier to check if a specific place is empty or not
 */
void flist_init(struct flist *flist) {
  int position = 0;
  while (position < MAP_SIZE) {
    flist_reset_position(flist, position);
    position++;
  }
  flist->initiated = true;
  DBG("flist_init - flist initiated!");
}

/**
 * Get the next free position
 * Iterates over the entire initiated list to find the next free position
 */
int flist_get_next_free_position(struct flist *flist) {
  int position = 0;
  int free_position = 0;
  while (position < MAP_SIZE) {
    // The files will begin at START_POSITION in the list
    free_position = START_POSITION + position;
    // Return the first position that is free
    if (flist->content[free_position] == NULL) {
      break;
    }
    position++;
  }
  DBG("flist_get_next_free_position - free_position: %i", free_position);
  return free_position;
}

/**
 * Sets the given index to NULL in the list
 */
void flist_reset_position(struct flist *flist, int content_index) {
//  flist->content[content_index].file = NULL;
//  flist->content[content_index].process_id = (uint32_t) NULL;

  // Close the file if it is opened
  if (flist->content[content_index] != NULL) {
    file_close(flist->content[content_index]);
  }
  // "Reset" the index
  flist->content[content_index] = NULL;
}

/**
 * A function that given a file (struct file*, see filesys/file.h)
 * and a process id INSERT this in a list of files. Return an
 * integer that can be used to find the opened file later.
 */
int flist_insert(struct flist *flist, struct file *file) {
  if (flist_can_insert(flist)) {
    int file_position = flist_get_next_free_position(flist);

//    flist->content[file_position].file = file;
//    flist->content[file_position].process_id = process_id;
    flist->content[file_position] = file;

    return file_position;
  }
  return -1;
}

/**
 * A function that given an integer (obtained from above function)
 * and a process id FIND the file in a list. Should return NULL if
 * the specified process did not insert the file or already removed
 * it.
 */
struct file* flist_get_from_index(struct flist *flist, int fd_index) {
  struct file* file = flist->content[fd_index];
//  if (flist->content[fd_index].process_id == process_id) {
//    file = flist->content[fd_index].file;
//  }
  DBG("flist_get_from_index - fd_index: %i, file: %p", fd_index, file);
  return file;
}

struct file* flist_get_from_fd(struct flist *flist, int fd_index) {
  return flist->content[fd_index];
}

/**
 * A function that given an integer (obtained from above function)
 * and a process id REMOVE the file from a list. Should return NULL
 * if the specified process did not insert the file or already
 * removed it.
 */
int flist_remove(struct flist *flist, int fd_index) {
  struct file* file = flist->content[fd_index];
  if (file != NULL) {
    DBG("flist_remove - fd_index: %i", fd_index);
    flist_reset_position(flist, fd_index);
    return fd_index;
  }
  DBG("flist_remove - Could not remove the file! arg fd_index: %i, saved file address: %p", fd_index, file);
  return NULL;
}

/**
 * A function that given a process id REMOVE ALL files the specified
 * process have in the list.
 */
void flist_remove_all(struct flist *flist) {
  int position = 0;
  while (position < MAP_SIZE) {
    flist_reset_position(flist, position);
    position++;
  }
}
