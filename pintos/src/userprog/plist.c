#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <threads/malloc.h>

#include "plist.h"

#define DBG(format, ...) printf("# PLIST - DEBUG: " format "\n", ##__VA_ARGS__)
//#define DBG(format, ...)

/**
 * Initiate the p-list to default values
 */
void plist_init(struct plist *p_list) {
  lock_init(&p_list_lock);
  int position = 0;
  while (position < PLIST_SIZE) {
//    plist_reset_position(p_list, position);
    p_list->list[position].id = (int) NULL;
    p_list->list[position].free = true;
    p_list->list[position].parent_id = -1;
    p_list->list[position].exit_status = -1;
//    cond_init(&p_list->list[position].cond);
    lock_init(&p_list->list[position].p_lock);
    sema_init(&p_list->list[position].p_sema, 0);
    position++;
  }

  DBG("plist_init - iiiiiiiiiiiiiiiiinitiated!");
}

/**
 * Reset the specified position to default values
 */
void plist_reset_position(struct plist *p_list, int position) {
  size_t a = 64;

  struct plist_info *current = &p_list->list[position];
  DBG("plist_reset_position - %i", position);
  current->id = -1;
  current->parent_id = -1;
  current->name = malloc(a);
  strlcpy(current->name, "www", a);
//  current->name = "";
  current->exit_status = -1;
  current->free = true;
  current->alive = false;
  current->parent_alive = false;


//  struct plist_info current = p_list->list[position];
//  current.id = -1;
//  current.parent_id = -1;
//  current.name = malloc(a);
//  strlcpy(current.name, "", a);
//  current.exit_status = -1;
//  current.free = true;
//  current.alive = false;
//  current.parent_alive = false;
//  DBG("plist_reset_position - Name: %s", current.name);
}

void plist_make_available(struct plist *p_list, int position) {
  p_list->list[position].free = true;
}
void plist_kill(struct plist *p_list, int position) {
  p_list->list[position].alive = false;
}
void plist_kill_parent(struct plist *p_list, int position) {
  p_list->list[position].parent_alive = false;
}

void plist_change_position_exit_status(struct plist *p_list, int position, int new_exit_status) {
  struct plist_info *current = &p_list->list[position];
  DBG("plist_change_position_exit_status - Process ID: %i, current exit status: %i, new exit status: %i",
      current->id,
      current->exit_status,
      new_exit_status);
//  p_list->list[position].exit_status = new_exit_status;
  current->exit_status = new_exit_status;
  plist_print_position(position, current, true);
}

bool plist_parent_alive(struct plist *p_list, int parent_id) {
//  lock_acquire(&p_list_lock);

  int position = 0;
  bool parent_alive = false;

  while (position < PLIST_SIZE) {
    struct plist_info *current = &p_list->list[position];
    if (current->id == parent_id) {
      parent_alive = true;
      break;
    }
    position++;
  }

//  lock_release(&p_list_lock);
  return parent_alive;
}

/**
 * Set the children process' parent as not alive, that belong to the parent_id-process
 */
void plist_kill_parent_mark(struct plist *p_list, int parent_id) {
  int position = 0;

  while (position < PLIST_SIZE) {
    struct plist_info *current = &p_list->list[position];
    if (!current->alive && current->parent_id == parent_id) {
//      plist_reset_position(p_list, position);
//      plist_make_available(p_list, position);
      current->parent_alive = false;
      current->free = true;
//      sema_up(&current->p_sema);
    }
    position++;
  }
}

/**
 * Iterate through the entire list to find the next free position in the p-list
 */
int plist_get_next_free_position(struct plist *p_list) {
  lock_acquire(&p_list_lock);
  int position = 0;
  bool position_found = false;
  while (position < PLIST_SIZE) {
    if (p_list->list[position].free) {
      position_found = true;
      break;
    }
    position++;
  }

  // TODO Ask! Is it OK that the release is here and not in the if-statement?
  lock_release(&p_list_lock);

  if (position_found) {
    DBG("plist_get_next_free_position - free position: %i", position);
    return position;
  } else {
    DBG("plist_get_next_free_position - THE LIST IS FULL!");
    return -1;
  }
}

/**
 * Insert information about a process to the first free position
 */
//int plist_insert(struct plist *p_list, int process_id, int process_parent_id, char *process_name, int process_exit_status) {
int plist_insert(struct plist *p_list, int process_id, int process_parent_id, char *process_name) {
  int position = plist_get_next_free_position(p_list);

  lock_acquire(&p_list_lock);

  DBG("plist_insert - position: %i", position);

  if (position != -1) {
    DBG("plist_insert - Inserting!");
    DBG("plist_insert - Process ID: %i, process parent ID: %i, process name: %s",
        process_id,
        process_parent_id,
        process_name);

    // Reset the position
    plist_reset_position(p_list, position);

    struct plist_info *free_plist = &p_list->list[position];
    free_plist->id = process_id;
    free_plist->parent_id = process_parent_id;
//    free_plist->name = process_name;
    strlcpy(free_plist->name, process_name, PLIST_P_NAME);
//    free_plist->exit_status = process_exit_status;
    free_plist->free = false;
    free_plist->alive = true;
    free_plist->parent_alive = true;

  } else {
    DBG("\n\nplist_insert - Could not insert!\n\n");
  }

  lock_release(&p_list_lock);
  return position;
}

/**
 * Get a specified position from the p-list
 */
struct plist_info *plist_get(struct plist *p_list, int position) {
  if (position > PLIST_SIZE) {
    return NULL;
  }
  return &p_list->list[position];
}

int plist_is_parent_to(struct plist *p_list, int child_process_id, int parent_id) {
  int child_process_position = -1;
  int position = 0;

  DBG("plist_is_parent_to - process ID: %i, parent ID; %i", child_process_id, parent_id);

  while (position < PLIST_SIZE) {
    struct plist_info *current = &p_list->list[position];
    if (current->id == child_process_id) {
      plist_print_position(position, current, true);
      if (current->parent_id == parent_id) {
        DBG("plist_is_parent_to - Found process! Current parent ID: %i", current->parent_id);
  //      parent_to = current->parent_id == parent_id;
        child_process_position = position;
//        sema_down(&current->p_sema);
      } else {
        DBG("plist_is_parent_to - Process found but parent ID is not the same! Current parent ID: %i", current->parent_id);
      }
      break;
    }
    position++;
  }

  DBG("plist_is_parent_to - position: %i", child_process_position);

  return child_process_position;
}

/**
 * Remove a specific position from the p-list
 */
void plist_remove(struct plist *p_list, int position, int thread_id) {
  lock_acquire(&p_list_lock);
  struct plist_info *current = &p_list->list[position];

  DBG("plist_remove - Removing from position: %i", position);
  DBG("plist_remove - Argument process ID: %i", thread_id);
  DBG("plist_remove - Process ID: %i, parent process ID: %i, process name: %s, parent alive: %i, alive: %i, free: %i",
      current->id,
      current->parent_id,
      current->name,
      current->parent_alive,
      current->alive,
      current->free);

  if (current->free) {
    DBG("\n\n\n\nplist_remove - Fail!");
    plist_print(p_list);
    DBG("plist_remove - Fail!\n\n\n\n");
  } else {
    plist_kill_parent_mark(p_list, current->id);
    current->parent_alive = plist_parent_alive(p_list, current->parent_id);
    current->alive = false;

    // If the parent isn't alive we can make the position available
    if (!current->parent_alive) {
      plist_make_available(p_list, position);
    }

    DBG("plist_remove - Success!");
    DBG("plist_remove - Process ID: %i, parent process ID: %i, process name: %s, parent alive: %i, alive: %i, free: %i",
        current->id,
        current->parent_id,
        current->name,
        current->parent_alive,
        current->alive,
        current->free);
    sema_up(&current->p_sema);
  }

//  struct plist_info current_plist = p_list->list[position];
//  if (current_plist.free) {
//    DBG("\n\nplist_remove - Fail!");
//    DBG("plist_remove - Position process ID: %i, parent ID: %i", current_plist.id, current_plist.parent_id);
//    DBG("plist_remove - Argument process ID: %i", thread_id);
//    plist_print(p_list);
//    DBG("plist_remove - Fail!\n\n");
//  } else {
//    DBG("plist_remove - Success!");
//    DBG("plist_remove - Process ID: %i, parent process ID: %i, process name: %s",
//        current_plist.id,
//        current_plist.parent_id,
//        current_plist.name);
//    plist_make_available(p_list, position);
//  }

  DBG("plist_remove - DONE Removing from position: %i", position);
  lock_release(&p_list_lock);
}

int plist_get_size(struct plist *p_list) {
  int position = 0;
  int size = 0;
  while (position < PLIST_SIZE) {
//    if (p_list[position].id != -1) {
    if (!p_list->list[position].free) {
      size++;
    }
    position++;
  }
  return size;
}

void plist_print_position(int position, struct plist_info *current, bool frame) {
  char end_char = '#';

  if (frame) {
    DBG("F##################### F - R - A - M - E ####################################F");
    DBG("R POS  | ID   | NAME                | P_ID | P_ALIVE | FREE | ALIVE | EXIT_S R");
    DBG("A -------------------------------------------------------------------------- A");
    end_char = 'A';
  }

  DBG("%c %-5d| %-5d| %-20s| %-5d| %-8d| %-5d| %-6d| %-7d%c",
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
    DBG("M                                                                            M");
    DBG("E##################### F - R - A - M - E ####################################E");
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

  DBG("##############################################################################");
  DBG("# POS  | ID   | NAME                | P_ID | P_ALIVE | FREE | ALIVE | EXIT_S #");
  DBG("# -------------------------------------------------------------------------- #");
  while (position < PLIST_SIZE) {
    struct plist_info *current = &p_list->list[position];
    if (current->id != (int) NULL) {
//      DBG("# %-5d| %-5d| %-20s| %-5d| %-8d| %-5d| %-6d#",
//          position,
//          current->id,
//          current->name,
//          current->parent_id,
//          current->parent_alive,
//          current->free,
//          current->alive);
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
  DBG("# -------------------------------------------------------------------------- #");
  DBG("# Total list size: %-57i #", total_size);
  DBG("# Total alive count: %-55i #", alive);
  DBG("# Total dead count: %-56i #", dead);
  DBG("# Total zombie count: %-54i #", zombies);
  DBG("#                                                                            #");
  DBG("##############################################################################");
}
