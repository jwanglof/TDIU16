#ifndef THREAD_H
#define THREAD_H

struct thread
{
  /* This is just a part of the Pintos Thread. */
  void* pagedir;
};

/* Initiate the threading system. */
void thread_init();
  
/* Return a pointer to the currently active thread. */
struct thread* thread_current();

#endif
