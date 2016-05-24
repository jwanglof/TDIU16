#include <syscall-nr.h>
#include <stdio.h>
#include <stdint.h>
#include "tests/lib.h"
#include "tests/main.h"

/**
 * From threads/vaddr.h:
 */
#define BITMASK(SHIFT, CNT) (((1ul << (CNT)) - 1) << (SHIFT))

#define PGSHIFT 0                          /* Index of first offset bit. */
#define PGBITS  12                         /* Number of offset bits. */
#define PGSIZE  (1 << PGBITS)              /* Bytes in a page. */
#define PGMASK  BITMASK(PGSHIFT, PGBITS)   /* Page offset bits (0:12). */

static inline void *pg_round_up (const void *va) {
  return (void *) (((uintptr_t) va + PGSIZE - 1) & ~PGMASK);
}

/**
 * A global variable that will give us an address in the BSS segment.
 */
int global = 3;

void test_main(void)
{
	// Get the addres of the first unmapped page in the system.
	unsigned page = (unsigned)pg_round_up(&global);

	// Reserve space for 3 parameters (write requires 4).
	page -= sizeof(int) * 4;

	asm volatile (
		"movl %%esp, %%edi;"
		"movl %0, %%esp;"       // Set stack pointer to right below page boundary.
		"movl %1, (%%esp);"     // Try to call SYS_WRITE
		"movl %2, 4(%%esp);"    // Write to STDOUT
		"movl %3, 8(%%esp);"    // Load buffer.
		//"movl $5, 12(%%esp);" // Can not write the last parameter as we would get a pagefault.
		"int $0x30;"
		"movl %%edi, %%esp;"    // Restore esp in case we do not crash (as we should).
		:
		: "r" (page),
		  "i" (SYS_WRITE),
		  "i" (STDOUT_FILENO),
		  "i" ("TEST\n")
		: "%esp", "%eax", "%edi");

	fail("should have died.");
}
