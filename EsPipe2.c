/*#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/wait.h>
#define BUFFER_SIZE 1024
int main(int argc, char *argv[])
{
    FILE *file_sorgente;
    FILE *file_arrivo;
    char buffer[BUFFER_SIZE];
    file_arrivo=fopen(argv[2],'w');
    int fd[2];
    pipe(fd);
    int p=fork();
    if(p>0)//padre
    {   
        int n;
        close(fd[0]);
        file_sorgente=fopen(argv[1],'r');
        while (n=fread(buffer,1,sizeof(buffer), file_sorgente)>0);
        {
            write(fd[1])
        }
        
    }
    return 1;
}*/