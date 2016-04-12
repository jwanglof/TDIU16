#include <stddef.h>
#include <stdio.h>

#include "flist.h"

#define DBG(format, ...) printf("# FLIST - DEBUG: " format "\n", ##__VA_ARGS__)
//#define DBG(format, ...)

bool flist_is_not_full(struct flist *flist) {
  return flist->total_files < MAP_SIZE;
}

bool flist_can_insert(struct flist *flist) {
  return flist_is_not_full(flist) && flist->initiated;
}

void flist_init(struct flist *flist) {
  flist->initiated = true;
//  flist->next_free_position = 50;
  int position = 0;
  while (position < MAP_SIZE) {
    flist_reset_position(flist, position);
    position++;
  }
  flist->total_files = 0;
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

void flist_reset_position(struct flist *flist, int content_index) {
  flist->content[content_index].file = NULL;
  flist->content[content_index].process_id = (uint32_t) NULL;
}

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
  // TODO Why do we need the process_id?
//  if (flist->content[index].process_id != NULL) {
//    return flist->content[index].file;
//  }
//  return NULL;
  struct file* get_file = flist->content[fd_index].file;
  DBG("flist_get_from_index - process_id: %i, fd_index: %i, file: %p", process_id, fd_index, get_file);
  return get_file;
}

/**
 * A function that given an integer (obtained from above function)
 * and a process id REMOVE the file from a list. Should return NULL
 * if the specified process did not insert the file or already
 * removed it.
 */
int flist_remove(struct flist *flist, int process_id, int fd_index) {
  if (flist->content[fd_index].file != NULL) {
    DBG("flist_remove - process_id: %i, fd_index: %i", process_id, fd_index);
    flist_reset_position(flist, fd_index);
    flist->total_files--;
    return fd_index;
  }
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
    }
    position++;
  }
}
