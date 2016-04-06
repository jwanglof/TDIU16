#include <stdio.h>
#include <string.h>

/*
 * Skriv ett program som går igenom alla argument som gavs till main på kommandoraden. Skriv ut
 * varje argument (ord) i en välformaterad lista. Först på varje rad skriver du ut argumentet
 * vänsterjusterat och sedan skriver du ut längden (antalet tecken) på argumentet högerjusterat. Du får
 * anta att alla argument är som längst 20 tecken. Slutligen skall du skriva ut totala antalet tecken
 * (heltal) och genomsnittliga antalet tecken per argument (flyttal) med två decimalers noggrannhet.
 * Programmet skall returnera totala antalet tecken. Ditt program skall överensstämma med
 * testkörningarna nedan
 */

int main(int argc, char* argv[]) {
    int total_chars = 0;
    int total_args = 0;
    int i;
    for (i = 0; i < argc; i++) {
        int arg_length = strlen(argv[i]);
        total_chars += arg_length;
        total_args += 1;
        printf("%-20s %i\n", argv[i], arg_length);
    }
    printf("%-20s %i\n", "Total length", total_chars);
    printf("%-20s %i\n", "Average length", (total_chars / total_args));

    return total_chars;
}