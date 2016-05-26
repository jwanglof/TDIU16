#ifndef PAGEDIR_H
#define PAGEDIR_H

typedef int bool;
#define false 0
#define true 1

/* Returnerar true om adressen innehåller ett noll-tecken, '\0'. */
bool is_end_of_string(char* adr);

/* Returnerar första adressen i samma sida som adr. */
void* pg_round_down(const void* adr);

/* Returnerar numret på sidan som innehåller adr. Sidnummer börjar
 * räkna på 0, som allt annat i C. */
unsigned pg_no(const void* adr);

/* Använder översättningstabellen för att slå upp fysisk adress
 * motsvarande adr. Om översättningen misslyckas returneras NULL.
 * Första parametern anger den översättningstabell som skall
 * användas. Den hittar du normalt i den datatyp som motsvarar PCB.
 * Titta igenom de givna headerfilerna.
 */
void *pagedir_get_page (void *pagetable, const void *adr);

/* Antal byte i varje sida (page). Page size. */
#define PGSIZE 100

/* Funktioner som används för att simulera ett system och testa dina
 * funktioner. */
void start_evaluate_algorithm(void* start, int size);
void evaluate(bool result);
void end_evaluate_algorithm();

/* This allow to simulate different pagefault time. Does not exist in
 * Pintos. */
void simulator_set_pagefault_time(unsigned i);

#endif
