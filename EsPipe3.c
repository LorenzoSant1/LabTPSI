#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


struct Libro {
   char titolo[100];
   char autore[100];
   int prezzo;
};


int main() {
   int fd[2];
   int p;
   int status;
   struct Libro libro1, libro2;
   if (pipe(fd) == -1) {
       printf("Errore nella creazione della pipe");
       exit(1);
   }


   p = fork();


   if (p < 0) {
       printf("Errore nella fork");
       exit(1);
   } else if (p == 0) {  // Processo figlio
       close(fd[1]);


     
       read(fd[0], &libro1, sizeof(libro1));
       read(fd[0], &libro2, sizeof(libro2));


       printf("Libro 1:\n");
       printf("Titolo: %s\n", libro1.titolo);
       printf("Autore: %s\n", libro1.autore);
       printf("Prezzo: %d\n", libro1.prezzo);


       printf("Libro 2:\n");
       printf("Titolo: %s\n", libro2.titolo);
       printf("Autore: %s\n", libro2.autore);
       printf("Prezzo: %d\n", libro2.prezzo);


       close(fd[0]);
   } else {  // Processo padre
       close(fd[0]);


       printf("Inserire i nomi dei libri\n");
       printf("nome libro 1:\n");
       scanf("%s",&libro1.titolo);
       printf("nome libro 2:\n");
       scanf("%s",&libro2.titolo);
       printf("Inserire i nomi degli autori dei libri\n");
       printf("nome autore libro 1:\n");
       scanf("%s",&libro1.autore);
       printf("nome autore libro 2:\n");
       scanf("%s",&libro2.autore);
       printf("Inserire i prezzi dei libri\n");
       printf("prezzo libro 1:\n");
       scanf("%d",&libro1.prezzo);
       printf("prezzo libro 2:\n");
       scanf("%d",&libro2.prezzo);
       write(fd[1], &libro1, sizeof(libro1));
       write(fd[1], &libro2, sizeof(libro2));


       close(fd[1]);
       wait(&status);
   }


   return 0;
}









