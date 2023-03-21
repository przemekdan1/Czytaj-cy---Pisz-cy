#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX 20 // pula buforow
#define MAX2 24 // rozmiar pamieci dzielonej

//przydzielanie wartosci komunikatow
#define W1 1
#define W2 2
#define W3 3
#define SC 4
#define SP 5

#define P 20 // ilosc procesow

//struktura komunikatu
struct bufor
{
    long mtype;
    int mvalue;
};

int shmID, msgID; //identfikatory

//obsluga handlera
void koniec(int sig)
{
   msgctl(msgID, IPC_RMID, NULL);
   shmctl(shmID, IPC_RMID, NULL);
   printf("MAIN - funkcja koniec sygnal %d: Koniec.\n",sig);
   exit(1);
}


int main()
{
    struct sigaction act;
    act.sa_handler = koniec;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);

    key_t klucz_kk, klucz_pd; // klucze kolejki i pamieci dzielonej
    struct bufor komunikatSP, komunikatSC, komunikatW1, komunikatW2, komunikatW3;
    komunikatW1.mtype = W1;
    komunikatW2.mtype = W2;
    komunikatW3.mtype = W3;
    komunikatSC.mtype = SC;
    komunikatSP.mtype = SP;
    int i;


    if ((klucz_kk = ftok(".", 'A')) == -1) // Stworzenie klucza do kolejki komunikatow
    {
        printf("Blad ftok KK\n");
        exit(1);
    }
    if ((msgID = msgget(klucz_kk, IPC_CREAT | IPC_EXCL | 0666))== -1) // Stworzenie kolejki komunikatow
    {
        printf("blad kolejki komunikatow\n");
        exit(1);
    }


    if ((klucz_pd = ftok(".",'B')) == -1) // Stworzenie klucza do pamieci dzielonej
    {
        printf("Blad ftok PD mainprog\n");
        exit(1);
    }
    if((shmID = shmget(klucz_pd, MAX2 * sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {
        printf("blad pamieci dzielonej\n");
        exit(1);
    }



    // komunikat W1;
    for(i = 0; i < MAX; i++)
    {
        if(msgsnd(msgID, &komunikatW1, sizeof(komunikatW1.mvalue), 0) == -1)
        {
            printf("blad wyslania kom. pustego\n");
            exit(1);
        }
    }

    // komunikat W2;
    for(i = 0; i < MAX; i++)
    {
        if(msgsnd(msgID, &komunikatW2, sizeof(komunikatW2.mvalue), 0) == -1)
        {
            printf("blad wyslania kom. pustego\n");
            exit(1);
        }
    }

    // komunikat W3;
    for(i = 0; i < MAX; i++)
    {
        if(msgsnd(msgID, &komunikatW3, sizeof(komunikatW3.mvalue), 0) == -1)
        {
            printf("blad wyslania kom. pustego\n");
            exit(1);
        }
    }

    // komunikat SC;
    for(i = 0; i < MAX; i++)
    {
        if(msgsnd(msgID, &komunikatSC, sizeof(komunikatSC.mvalue), 0) == -1)
        {
            printf("blad wyslania kom. pustego\n");
            exit(1);
        }
    }

    // komunikat SP;
    for(i = 0; i < MAX; i++)
    {
        if(msgsnd(msgID, &komunikatSP, sizeof(komunikatSP.mvalue), 0) == -1)
        {
            printf("blad wyslania kom. pustego\n");
            exit(1);
        }
    }

    for (i = 0; i < P; i++) // Tworzenie P procesow piszacych
    {
        switch (fork())
        {
        case -1:
            perror("Blad fork (mainprog)");
            exit(2);
        case 0:
            execl("./pisz","pisz", NULL);
        }
    }
    
    for (i = 0; i < P; i++) // Tworzenie P procesow czytajacych
    {
        switch (fork())
        {
        case -1:
            printf("Blad fork (mainprog)\n");
            exit(2);
        case 0:
            execl("./czyt","czyt",NULL);
        }
    }

    for (i = 0; i < 2 * P; i++)
    wait(NULL);

    msgctl(msgID, IPC_RMID, NULL);
    shmctl(shmID, IPC_RMID, NULL);
    printf("MAIN: Koniec.\n");
}
