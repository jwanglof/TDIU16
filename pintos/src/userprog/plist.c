#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <threads/malloc.h>

#include "plist.h"

// #define DBG(function_name, format, ...) printf("# PLIST (" function_name "): " format "\n", ##__VA_ARGS__)
#define DBG(function_name, format, ...)

struct plist_info *plist_get_position(struct plist *p_list, int position) {
  return &p_list->list[position];
}

struct plist_info *plist_get_process(struct plist *p_list, int process_id) {
  DBG("plist_get_process", "ENTERED, ID: %i", process_id);
  int w_position = 0;
  struct plist_info *process = NULL;
  while (w_position < PLIST_SIZE) {
    struct plist_info *current = plist_get_position(p_list, w_position);

    if (current->id == process_id) {
      process = current;
      break;
    }

    w_position++;
  }
  DBG("plist_get_process", "DONE, ID: %i, is NULL: %i, position: %i", process_id, process == NULL, w_position);

  return process;
}

/**
 * Check if the parent is alive by checking the list for the parent process
 */
bool plist_check_if_parent_alive(struct plist *p_list, int parent_id) {
//  lock_acquire(&p_list_lock);

  DBG("plist_check_if_parent_alive", "ENTERED, parent ID: %i", parent_id);

  struct plist_info *current = plist_get_process(p_list, parent_id);

  DBG("plist_check_if_parent_alive", "is NULL: %i", current == NULL);

  bool is_alive = false;
  if (current != NULL) {
    is_alive = current->alive;
  }

//  lock_release(&p_list_lock);
  DBG("plist_check_if_parent_alive", "DONE, parent ID: %i, is alive: %i", parent_id, is_alive);

  return is_alive;
}

/**
 * Go through the entire list
 * If the child have parent_id as parent we will mark the parent as not alive
 */
void plist_kill_children(struct plist *p_list, int parent_id) {
  int w_position = 0;
  while (w_position < PLIST_SIZE) {
    struct plist_info *current = plist_get_position(p_list, w_position);
    if (current->parent_id == parent_id) {
      current->parent_alive = false;
      if (!current->alive) {
        DBG("plist_kill_children", "Will free child! Process ID: %i, position: %i, parent ID: %i", current->id, w_position, current->parent_id);
        current->free = true;
      }
    }
    w_position++;
  }
}

/**
 * Change the exit status on a process in the list
 */
void plist_change_exit_status(struct plist *p_list, int process_id, int new_exit_status) {
  lock_acquire(&p_list_lock);
  DBG("plist_change_exit_status", "ENTERED, ID: %i, new exit status: %i", process_id, new_exit_status);
  struct plist_info *current = plist_get_process(p_list, process_id);
  if (current != NULL) {
    DBG("plist_change_exit_status", "DONE, ID: %i is NOT NULL", process_id);
    current->exit_status = new_exit_status;
    // Add sema_up() here instead!
  } else {
    DBG("plist_change_exit_status", "DONE, ID: %i is NULL!!!", process_id);
  }
  DBG("plist_change_exit_status", "DONE, ID: %i", process_id);
  lock_release(&p_list_lock);
//  sema_down(&current->p_sema);
}

/**
 * Get the exit status for a process in a list
 * Will run sema_down() since it will be a parent process that runs this function and it will wait for the
 *   child process ID that is passed to this function
 * Will run sema_up() in the remove-function
 */
int plist_process_exit_status_get(struct plist *p_list, int process_id) {
  struct plist_info *current = plist_get_process(p_list, process_id);
  DBG("plist_process_exit_status_get", "ENTERED, ID: %i", process_id);
  int exit_status = -1;
  if (current != NULL) {
    DBG("plist_process_exit_status_get", "NOT NULL, ID: %i", process_id);
    sema_down(&current->p_sema);
    DBG("plist_process_exit_status_get", "BELOW SEMA, ID: %i", process_id);
    exit_status = current->exit_status;
    // Free the process since it's run-time is done
    current->free = true;
  }
  DBG("plist_process_exit_status_get", "DONE, ID: %i, status: %i", process_id, exit_status);

  return exit_status;
}

/**
 * Initiate the list to default values
 */
void plist_init(struct plist *p_list) {
  lock_init(&p_list_lock);

  lock_acquire(&p_list_lock);
  DBG("plist_init", "ENTERED");
  int w_position = 0;
  while (w_position < PLIST_SIZE) {
    struct plist_info *current = plist_get_position(p_list, w_position);
    current->free = true;
    w_position++;
  }
  DBG("plist_init", "DONE");
  lock_release(&p_list_lock);

  // Insert dummie parent processes
  /*int a = plist_insert(p_list, 1, "lolz", 0);
  int b = plist_insert(p_list, 2, "lolz#2", 0);
  struct plist_info *aa = plist_get_position(p_list, a);
  aa->parent_alive = true;
  struct plist_info *bb = plist_get_position(p_list, b);
  bb->parent_alive = true;*/
}


/**
 * A function that given process information (up to you to create) inserts this in a list of running processes
 * and return an integer that can be used to find the information later on.
 */
int plist_insert(struct plist *p_list, int process_id, char process_name[], int parent_id) {
  lock_acquire(&p_list_lock);

  DBG("plist_insert", "ENTERED, ID: %i, name: %s, parent ID: %i", process_id, process_name, parent_id);

  int w_position = 0;
  int index = -1;
  while (w_position < PLIST_SIZE) {
    struct plist_info *current = plist_get_position(p_list, w_position);
    // Add the information if the current position is free
    if (current->free) {
      current->free = false;
      current->alive = true;
      current->id = process_id;
      current->parent_id = parent_id;
      current->parent_alive = plist_check_if_parent_alive(p_list, parent_id);
      current->exit_status = -1;
//      current->name = malloc(PLIST_P_NAME);
      strlcpy(current->name, process_name, PLIST_P_NAME);
      sema_init(&current->p_sema, 0);
      index = w_position;
      break;
    }
    w_position++;
  }

  DBG("plist_insert", "index: %i", index);
  DBG("plist_insert", "DONE, ID: %i, name: %s, parent ID: %i", process_id, process_name, parent_id);

  lock_release(&p_list_lock);

  return index;
}

/**
 * A function that given an integer (obtained from above function) FIND the process information in the list. Should
 * return some failure code if no process matching the integer is in the list. Or, optionally, several functions to
 * access any information of a particular process that you currently need.
 */
struct plist_info *plist_find(struct plist *p_list, int process_id) {
  lock_acquire(&p_list_lock);
  DBG("plist_find", "ENTERED, ID: %i", process_id);
//  plist_print(p_list);
  struct plist_info *current = plist_get_process(p_list, process_id);
  DBG("plist_find", "DONE, ID: %i, found: %i", process_id, current != NULL);
  lock_release(&p_list_lock);
  return current;
}

/**
 * A function that given an integer REMOVE the process information from the list. Should only remove the
 * information when no process or thread need it anymore, but must guarantee it is always removed EVENTUALLY.
 */
void plist_remove(struct plist *p_list, int process_id) {
  lock_acquire(&p_list_lock);

  DBG("plist_remove", "ENTERED, ID: %i", process_id);

  struct plist_info *current = plist_get_process(p_list, process_id);
  if (current != NULL) {
    current->alive = false;

    // Need to check if parent_id is alive
    // If it is alive, we can't free the position
    // If it isn't alive, we can free the position
    if (!plist_check_if_parent_alive(p_list, current->parent_id)) {
      current->free = true;
    }

    // Need to tell all the children that this process (parent) is dead
    plist_kill_children(p_list, current->id);

    sema_up(&current->p_sema);
  } else {
    DBG("plist_remove", "Current is NULL! ID: %i", process_id);
  }

  DBG("plist_remove", "DONE, ID: %i", process_id);

//  plist_print(p_list);

  lock_release(&p_list_lock);
}

void plist_print_position(int position, struct plist_info *current, bool frame) {
  char end_char = '#';

  if (frame) {
    DBG("", "F##################### F - R - A - M - E ####################################F");
    DBG("", "R POS  | ID   | NAME                | P_ID | P_ALIVE | FREE | ALIVE | EXIT_S R");
    DBG("", "A -------------------------------------------------------------------------- A");
    end_char = 'A';
  }

  DBG("", "%c %-5d| %-5d| %-20s| %-5d| %-8d| %-5d| %-6d| %-7d%c",
      end_char,
      position,
      current->id,
      current->name,
      current->parent_id,
      current->parent_alive,
      current->free,
      current->alive,
      current->exit_status,
      end_char);

  if (frame) {
    DBG("", "M E ################## F - R - A - M - E ################################# M E");
  }
}

/**
 * Print the list in a nice way
 */
void plist_print(struct plist *p_list) {
  int position = 0;
  int total_size = 0;
  int alive = 0;
  int dead = 0;
  int zombies = 0;

  DBG("", "##############################################################################");
  DBG("", "# POS  | ID   | NAME                | P_ID | P_ALIVE | FREE | ALIVE | EXIT_S #");
  DBG("", "# -------------------------------------------------------------------------- #");
  while (position < PLIST_SIZE) {
    struct plist_info *current = &p_list->list[position];
//    if (current->id != (int) NULL) {
    if (!current->free) {
      plist_print_position(position, current, false);

      if (current->alive) {
        alive++;
      } else {
        dead++;
        if (!current->free) {
          zombies++;
        }
      }

      total_size++;
    }
    position++;
  }
  DBG("", "# -------------------------------------------------------------------------- #");
  DBG("", "# Total list size: %-57i #", total_size);
  DBG("", "# Total alive count: %-55i #", alive);
  DBG("", "# Total dead count: %-56i #", dead);
  DBG("", "# Total zombie count: %-54i #", zombies);
  DBG("", "#                                                                            #");
  DBG("", "##############################################################################");
}
