#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#define DIM_ARRAY 5
int main()
{

    int fd[2];
    int fattore;
    int n;
    int p;
    int status;
    if(pipe(fd)==-1)
    {
        printf("Errore nella creazione della pipe\n");
        exit(-1);
    }
    p=fork();
    if(p<0)
    {
        printf("Erore nella fork\n");
        exit(-1);
    }
    if(p>0)
    {
        int numeri2[DIM_ARRAY];
        printf("Io sono il padre, il mio PID:%d, il PID di mio figlio è:%d\n", getpid(),p);  
        
       
        close(fd[1]);
        read(fd[0],numeri2,sizeof(numeri2));
                close(fd[0]);
        printf("Inserisci il numero da moltiplicare\n");
        scanf("%d", &fattore);
        for(int i=0;i<5;i++)
        {
            printf("Numero %d: %d\n",i+1,numeri2[i]*fattore);
        }
        wait(&status);

    }
    else
    {
        int numeri[DIM_ARRAY];
        printf("Io sono il figlio il mio PID:%d, il PID di mio padre è:%d\n",p, getppid());
        printf("Inserire numeri:\n");
        for(int i=0;i<5; i++)
        {
            printf("%d numero:", i+1);
            scanf("%d",&numeri[i]);    
        }    
        
        close(fd[0]);
            write(fd[1],numeri, sizeof(numeri));                    
        close(fd[1]);

    }
    return 1;
}
