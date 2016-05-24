#ifndef _PLIST_H_
#define _PLIST_H_


/* Place functions to handle a running process here (process list).
   
   plist.h : Your function declarations and documentation.
   plist.c : Your implementation.

   The following is strongly recommended:

   - A function that given process information (up to you to create)
     inserts this in a list of running processes and return an integer
     that can be used to find the information later on.

   - A function that given an integer (obtained from above function)
     FIND the process information in the list. Should return some
     failure code if no process matching the integer is in the list.
     Or, optionally, several functions to access any information of a
     particular process that you currently need.

   - A function that given an integer REMOVE the process information
     from the list. Should only remove the information when no process
     or thread need it anymore, but must guarantee it is always
     removed EVENTUALLY.
     
   - A function that print the entire content of the list in a nice,
     clean, readable format.
     
 */

#include <threads/synch.h>
#include <stdbool.h>

#define PLIST_SIZE 512
#define PLIST_P_NAME 64

typedef struct plist_info p_info;
struct plist_info {
  int id;
//  char *name;
  char name[PLIST_P_NAME];

  int exit_status;
  bool free;
  bool alive;

  int parent_id;
  bool parent_alive;

//  struct condition cond;
  struct lock p_lock;
  struct semaphore p_sema;
};

struct plist {
  p_info list[PLIST_SIZE];
};

//struct semaphore p_list_sema;
struct lock p_list_lock;

/**
 * A function that given process information (up to you to create) inserts this in a list of running processes
 * and return an integer that can be used to find the information later on.
 */
int plist_insert(struct plist *p_list, int process_id, char *process_name, int parent_id);

/**
 * A function that given an integer (obtained from above function) FIND the process information in the list. Should
 * return some failure code if no process matching the integer is in the list. Or, optionally, several functions to
 * access any information of a particular process that you currently need.
 */
struct plist_info *plist_find(struct plist *p_list, int process_id); // TODO Replace with position?

/**
 * A function that given an integer REMOVE the process information from the list. Should only remove the
 * information when no process or thread need it anymore, but must guarantee it is always removed EVENTUALLY.
 */
void plist_remove(struct plist *p_list, int process_id); // TODO Replace with position?

/**
 * A function that print the entire content of the list in a nice, clean, readable format.
 */
void plist_print_position(int position, struct plist_info *current, bool frame);
void plist_print(struct plist *p_list);

/**
 * Helpers
 */
struct plist_info *plist_get_position(struct plist *p_list, int position);
struct plist_info *plist_get_process(struct plist *p_list, int process_id);
bool plist_check_if_parent_alive(struct plist *p_list, int parent_id);
void plist_kill_children(struct plist *p_list, int parent_id);
void plist_change_exit_status(struct plist *p_list, int process_id, int new_exit_status);
int plist_process_exit_status_get(struct plist *p_list, int process_id);
void plist_init(struct plist *p_list);

#endif
