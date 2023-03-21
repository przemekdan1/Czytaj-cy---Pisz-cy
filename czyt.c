#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

struct bufor
{
    int mtype;
    int mvalue;
};

int *pam;

#define MAX 20
#define MAX2 24

#define odczyt pam[MAX]
#define zapis pam[MAX + 1]
#define liczba_piszacych pam[MAX + 2] // lp
#define liczba_czytajacych pam[MAX + 3] // lc

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

   fprintf(stderr, "-----Czytajacy-----\n");


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



   klucz_pd = ftok(".",'B');
   shmID = shmget(klucz_pd, MAX2*sizeof(int), IPC_CREAT|0666);
   pam = (int*) shmat(shmID, NULL, 0);


    msgrcv(msgID, &komunikatW3, sizeof(komunikatW3.mvalue), W3, 0); 		// wait(W3)
      msgrcv(msgID, &komunikatSC, sizeof(komunikatSC.mvalue), SC, 0);		// wait(SC)
        msgrcv(msgID, &komunikatW1, sizeof(komunikatW1.mvalue), W1, 0); 	// wait(W1)
          liczba_czytajacych = liczba_czytajacych + 1; // lc = lc + 1
          if(liczba_czytajacych == 1)
             msgrcv(msgID, &komunikatSP, sizeof(komunikatSP.mvalue), SP, 0);	// wait(SP)
        msgsnd(msgID, &komunikatW1, sizeof(komunikatW1.mvalue), 0); 		// signal(W1)
      msgsnd(msgID, &komunikatSC, sizeof(komunikatSC.mvalue), 0); 		// signal(SC)
    msgsnd(msgID, &komunikatW3, sizeof(komunikatW3.mvalue), 0); 		// signal(W3)

    fprintf(stderr, "Odczytal %d, bufor: %d\n", pam[odczyt], odczyt); 		// czytanie
    odczyt = (odczyt + 1) % MAX; 						// liczenie bufora

    msgrcv(msgID, &komunikatW1, sizeof(komunikatW1.mvalue), W1, 0);		// wait(W1)
      liczba_czytajacych = liczba_czytajacych - 1; 				// lc = lc - 1
      if (liczba_czytajacych == 0) 						// if lc = 0 then
         msgsnd(msgID, &komunikatSP, sizeof(komunikatSP.mvalue), 0); 		// signal(SP)
    msgsnd(msgID, &komunikatW1, sizeof(komunikatW1.mvalue), 0); 		// signal(W1)
}