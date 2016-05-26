#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
bool verify_fix_length(void* start, int length);
bool verify_variable_length(char* start);

#endif /* userprog/syscall.h */
