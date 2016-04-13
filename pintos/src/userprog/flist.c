#include <stddef.h>
#include <stdio.h>

#include "flist.h"

#define DBG(format, ...) printf("# FLIST - DEBUG: " format "\n", ##__VA_ARGS__)
//#define DBG(format, ...)

/**
 * Returns true if the list isn't full
 */
bool flist_is_not_full(struct flist *flist) {
  return flist->total_files < MAP_SIZE;
}

/**
 * Returns true if the list accepts new files
 */
bool flist_can_insert(struct flist *flist) {
  return flist_is_not_full(flist) && flist->initiated;
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
  flist->total_files = 0;
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
    if (flist->content[free_position].file == NULL) {
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
  flist->content[content_index].file = NULL;
  flist->content[content_index].process_id = (uint32_t) NULL;
}

/**
 * A function that given a file (struct file*, see filesys/file.h)
 * and a process id INSERT this in a list of files. Return an
 * integer that can be used to find the opened file later.
 */
int flist_insert(struct flist *flist, int process_id, struct file *file) {
  if (flist_can_insert(flist)) {
    int file_position = flist_get_next_free_position(flist);

    flist->content[file_position].file = file;
    flist->content[file_position].process_id = process_id;

    flist->total_files++;

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
struct file* flist_get_from_index(struct flist *flist, int process_id, int fd_index) {
  struct file* file = NULL;
  if (flist->content[fd_index].process_id == process_id) {
    file = flist->content[fd_index].file;
  }
  DBG("flist_get_from_index - process_id: %i, fd_index: %i, file: %p", process_id, fd_index, file);
  return file;
}

/**
 * A function that given an integer (obtained from above function)
 * and a process id REMOVE the file from a list. Should return NULL
 * if the specified process did not insert the file or already
 * removed it.
 */
int flist_remove(struct flist *flist, int process_id, int fd_index) {
  struct file* file = flist->content[fd_index].file;
  int file_process_id = flist->content[fd_index].process_id;
  if (file != NULL && file_process_id == process_id) {
    DBG("flist_remove - process_id: %i, fd_index: %i", process_id, fd_index);
    flist_reset_position(flist, fd_index);
    flist->total_files--;
    return fd_index;
  }
  DBG("flist_remove - Could not remove the file! arg process_id: %i, arg fd_index: %i, saved process_id: %i, saved file address: %p",
      process_id, fd_index, file_process_id, file);
  return NULL;
}

/**
 * A function that given a process id REMOVE ALL files the specified
 * process have in the list.
 */
void flist_remove_all(struct flist *flist, int process_id) {
  int position = 0;
  int flist_real_position;
  while (position < MAP_SIZE) {
    flist_real_position = START_POSITION + position;
    if (flist->content[flist_real_position].process_id == process_id) {
      flist_reset_position(flist, position);
      flist->total_files--;
    }
    position++;
  }
}
