#include <stdio.h>

/**
 * 1
 * Deklarerar ett heltal, ett flyttal, ett tecken, en text-sträng och en array med 5 heltal. Initiera alla
 * variabler direkt vid deklaration. En text-sträng, eller C-sträng, är en pekare till en sekvens av
 * tecken. Det är inte givet på förhand hur lång sekvensen är. Istället är det givet att sekvensen är
 * minst ett tecken, där det sista tecknet alltid måste vara det så kallade noll-tecknet (‘\0’). En tom
 * sträng innehåller alltså endast ett nolltecken. Nolltecknet brukar inte räknas med när längden på
 * en sträng beräknas. En tom sträng har alltså längden 0, även fast sekvensen för att lagra den är
 * ett tecken lång. [Not: #define SIZE 5]
 */
// Deklarerar ett heltal
int an_integer = 0;

// ett flyttal
float a_float = 1.0;

// ett tecken
char a_char = 'a';

// en text-sträng
char* a_string = "A string";

// en array med 5 heltal
int an_array[5] = {0, 1, 2, 3, 4};

/**
 * 5
 * Skapa ett funktion, du kan kalla den getline, som tar emot en array av tecken samt ett heltal som
 * anger arrayens storlek. Funktionen skall läsa in en rad med tecken från tangentbordet med
 * funktionen getchar från standardbiblioteket (man -s3c getchar). Rader avslutas med tecknet för
 * nyrad (‘\n’). Tecknen skall lagras i arrayen. Om arrayen är för liten avbryter du inläsningen av
 * raden i förtid. Avsluta i alla lägen arrayen med ett nolltecken. [Not: array som parameter]
 */
void own_getline(char characters[], int character_size) {

}

int main() {
    /**
     * 2
     * Använd funktionen printf från standardbiblioteket (man -s3c printf) för att skriva ut namn,
     * adress, och värde på varje variabel. Adresserna skall skrivas ut hexadecimalt, alltid med åtta
     * siffror (0x00010a08). Skriv ut innehållet i arrayvariablerna med formatet “%p” (dvs skriv ut
     * adressen till första värdet i arrayen, pekaren som anger var sekvensen värden startar, inte hela
     * arrayen). Strängen skriver du även ut med formatet “%s” som hanterar text-strängar speciellt.
     * Detta har direkt relevans för debugutskrifter i Pintos.
     * [Not: printf exempel, array vs pekare exempel, &array, array, &array[i], array[i], C-sträng]
     */
    // TODO Don't know how to print the name of the variable...
    printf("Integer:\n\tAddress: %p\n\tValue: %i\n\tName: ????", &an_integer, an_integer);
    printf("\n\n");

    printf("Float:\n\tAddress: %p\n\tValue: %f\n\tName: ????", &a_float, a_float);
    printf("\n\n");

    printf("Character:\n\tAddress: %s\n\tValue: %c\n\tName: ????", &a_char, a_char);
    printf("\n\n");

    printf("String:\n\tAddress: %p\n\tValue: %s\n\tName: ????", &a_string, a_string);
    printf("\n\n");

    printf("Integer array:\n\tAddress: %p\n\tValue: %p\n\tName: ????", &an_array, an_array);
    printf("\n\n");


    /**
     * 3
     * Använd en while-loop och pekarstegring för att skriva ut varje tecken i strängen. Först skall
     * adressen där tecknet är lagrat skrivas ut, sedan själva tecknet, ett tecken per rad. Med
     * pekarstegring avses att du varje varv i loopen ökar en startpekare att peka till nästa position i en
     * array. Innehållet på adressen hämtas som vanligt med * operatorn.
     */
    char *string_pointer = a_string;
    while (string_pointer != NULL && *string_pointer != '\0') {
        // TODO Not sure that %p is correct!
        printf("%p -- %c\n", &string_pointer, *string_pointer);
        string_pointer++;
    }
    printf("\n\n");


    /**
     * 4
     * Använd en for-loop och indexering för att skriva ut varje tecken i heltals-arrayen på samma
     * format som tecken arrayen. Med indexering avses att du använder indexoperatorn [].
     */
    int size_of_an_array = sizeof an_array / sizeof an_array[0];
    int i;
    for (i = 0; i < size_of_an_array; i++) {
        printf("%i\n", an_array[i]);
    }
    printf("\n\n");

    // TODO ???? 5
    int size = 0;
    printf("Charcters:");
    char c = getchar();


    return 0;
}
