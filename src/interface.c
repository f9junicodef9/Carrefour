#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

void message(int i, char* s) {
   #define COLONNE 20
   int j, NbBlanc;
   file = fopen("./test.txt", "a");
   NbBlanc=i*COLONNE;
   for (j=0; j<NbBlanc; j++) {
		putchar(' ');
		fprintf(file, " ");
	}
   printf("%s",s);
   fprintf(file, "%s", s);
   fflush(stdout);
   fclose(file);
}