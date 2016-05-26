#include <stdlib.h>
#include <stdio.h>
#include "pagedir.h"
#include "thread.h"


/* verfy_*_lenght är tänkta att användas i systemanrop som får in
 * opålitliga adresser från user mode. Operativsystemet ska inte kunna
 * luras att å processens vägnar använda adresser processen inte har
 * tillgång till. I pagedir.h finns funktioner som kan hjälpa dig slå
 * upp adresser i pagetable. Fundra hur du kan göra så få slagningar
 * som möjligt.
 *
 * Rekommenderat kompileringskommando:
 *
 *  gcc -Wall -Wextra -std=gnu99 -pedantic -g pagedir.o verify_adr.c
 */
//#error Read comment above and then remove this line.

/* Kontrollera alla adresser från och med start till och inte med
 * (start+length). */
/**
 * Kontrollera alla adresser från och med start till och inte med (start+length).
 * Returnerar true om alla adresser i intervallet är giltiga
 */
bool verify_fix_length(void* start, int length)
{
  printf("Length: %i\n", length);

  void* first_addr_in_page = pg_round_down(start);
  printf("First addr in page: %p (%i)\n", first_addr_in_page, (int)first_addr_in_page);

  int PAGE_OFFSET = (int)start % PGSIZE;
  printf("Offset: %i\n", PAGE_OFFSET);

  int amount_to_last_addr = length + PAGE_OFFSET;
  printf("Amount to last addr: %i\n", amount_to_last_addr);

  int amount_of_pages = amount_to_last_addr / PGSIZE;
  printf("Amount of pages: %i\n", amount_of_pages);

  // We need to have at least one page the address is in!
  if (amount_of_pages == 0) {
    amount_of_pages = 1;
    printf("NEW Amount of pages: %i\n", amount_of_pages);
  } else if (amount_to_last_addr % PGSIZE != 0) {
    // If the amount to the last address isn't 0 we need to add pages because we're trying to verify one more page
    amount_of_pages++;
    printf("NEW2 Amount of pages: %i\n", amount_of_pages);
  }

  int position = 0;
  while (position < amount_of_pages) {
    printf("First addr in page (loop): %p (%i)\n", first_addr_in_page, (int)first_addr_in_page);

    if (pagedir_get_page(thread_current()->pagedir, first_addr_in_page) == NULL) {
      return false;
    }
    first_addr_in_page += PGSIZE;
    position++;
  }
  return true;
}

/* Kontrollera alla adresser från och med start till och med den
 * adress som först innehåller ett noll-tecken, `\0'. (C-strängar
 * lagras på detta sätt.) */
bool verify_variable_length(char* start)
{
  // Default value for current page so we also check the very first page
  unsigned current_page = (unsigned) -1;
  while (true) {
    // If the current page isn't the same as the page the current char is in, verify it!
    if (current_page != pg_no(start)) {
      current_page = pg_no(start);

      // Verify the page
      if (pagedir_get_page(thread_current()->pagedir, start) == NULL) {
        return false;
      }
    }
    // If we have reached the end of the string, bail out
    if (is_end_of_string(start)) {
      return true;
    }
    start++;
  }
}

/* Definition av testfall */
struct test_case_t
{
  void* start;
  unsigned length;
};

#define TEST_CASE_COUNT 6

const struct test_case_t test_case[TEST_CASE_COUNT] =
{
  {(void*)100, 100}, /* one full page */
  {(void*)199, 102},
  {(void*)101, 98},
  {(void*)250, 190},
  {(void*)250, 200},
  {(void*)250, 210}
};

/* Huvudprogrammet utvärderar din lösning. */
int main(int argc, char* argv[])
{
  int i;
  bool result;

  if ( argc == 2 )
  {
    simulator_set_pagefault_time( atoi(argv[1]) );
  }
  thread_init();
  
  /* Testa algoritmen med ett givet intervall (en buffer). */
  for (i = 0; i < TEST_CASE_COUNT; ++i)
  {
    start_evaluate_algorithm(test_case[i].start, test_case[i].length);
    result = verify_fix_length(test_case[i].start, test_case[i].length);
    evaluate(result);
    end_evaluate_algorithm();
    printf("\n\n");
  }
    
  /* Testa algoritmen med en sträng. */
  for (i = 0; i < TEST_CASE_COUNT; ++i)
  {
    start_evaluate_algorithm(test_case[i].start, test_case[i].length);
    result = verify_variable_length(test_case[i].start);
    evaluate(result);
    end_evaluate_algorithm();
    printf("\n\n");
  }
  return 0;
}
