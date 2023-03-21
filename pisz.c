#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

struct bufor
{
    int mtype;
    int mvalue;
};

int *pam;
//dostepna pula buforow
#define MAX 20
#define MAX2 24


#define odczyt pam[MAX]
#define zapis pam[MAX + 1]
#define liczba_piszacych pam[MAX + 2] // lp
#define liczba_czytajacych pam[MAX + 3] // lc

//przydzielanie wartosci komunikatow
#define W1 1
#define W2 2
#define W3 3
#define SC 4
#define SP 5


int main()
{
    key_t klucz_kk, klucz_pd;
    int msgID, shmID;
    int i;
    struct bufor komunikatSP, komunikatSC, komunikatW1, komunikatW2, komunikatW3;
    komunikatSP.mtype = SP;
    komunikatSC.mtype = SC;
    komunikatW1.mtype = W1;
    komunikatW2.mtype = W2;
    komunikatW3.mtype = W3;


    fprintf(stderr, "-----Piszacy-----\n");

    //kolejka komunikatow
    if ((klucz_kk = ftok(".", 'A')) == -1)
    {
        printf("Blad ftok (A)\n");
        exit(2);
    }
    msgID = msgget(klucz_kk, IPC_CREAT | 0666);
    if (msgID == -1)
    {
      printf("blad klejki komunikatow\n");
      exit(1);
    }

    //pamiec dzielona
    klucz_pd = ftok(".", 'B');
    shmID = shmget(klucz_pd, MAX2 * sizeof(int), IPC_CREAT|0666);
    pam = (int*)shmat(shmID, NULL, 0); 


    msgrcv(msgID, &komunikatW2, sizeof(komunikatW2.mvalue), W2, 0); // wait(W2)
       liczba_piszacych = liczba_piszacych + 1; // lp = lp + 1
       if(liczba_piszacych == 1) 
	   msgrcv(msgID, &komunikatSC, sizeof(komunikatSC.mvalue), SC, 0); // wait(SC)
    msgsnd(msgID, &komunikatW2, sizeof(komunikatW2.mvalue), 0); // signal(W2)


    msgrcv(msgID, &komunikatSP, sizeof(komunikatSP.mvalue), SP, 0); // wait(SP)
    pam[zapis] = getpid(); // pisanie
    fprintf(stderr, "Zapisal %d\n", pam[zapis]); // pisanie
    zapis = (zapis + 1) % MAX; // liczenie bufora
    msgsnd(msgID, &komunikatSP, sizeof(komunikatSP.mvalue), 0); // signal(SP)



    msgrcv(msgID, &komunikatW2, sizeof(komunikatW2.mvalue), W2, 0); // wait(W2)
       liczba_piszacych = liczba_piszacych - 1; // lp = lp - 1
       if (liczba_piszacych == 0) // if lp = 0 then
           msgsnd(msgID, &komunikatSC, sizeof(komunikatSC.mvalue), 0); // signal(SC)
    msgsnd(msgID, &komunikatW2, sizeof(komunikatW2.mvalue), 0); // signal(W2)
}