#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MEM_SIZ 1024

	int N_GOODS = 40;
	
	int N_CUSTOMERS=20;
	int N_ROUNDS=1000000;
	int RATING=10;

	struct wsp_str{
		double Ratings[40]; 
		int NRatings[40];
		
	};
	struct wsp_str *pamiec;

int klient1();

int main()
{
	int kupionych=0;
	//deklaracja zmiennych w pamieci wspoldzielonej
	int id;
	struct wsp_str* Pamiec_WSP = 0;

	id = shmget(0x193, MEM_SIZ, IPC_CREAT |  0666);
	if (id == -1) 
	{
		perror("blad shmget");
		exit(errno);
	}

	Pamiec_WSP = (struct wsp_str *)shmat(id, 0, 0);
	if (Pamiec_WSP == (void *)-1) {
		perror("blad shmat");
		exit(errno);
	}
	pamiec = Pamiec_WSP;
	
	//wypelnianie zerami
	for(int i=0; i<N_GOODS; i++)
	{
		pamiec->Ratings[i]=0;
		pamiec->NRatings[i]=0;
		//printf("%d\t%f\t%d\n",i,pamiec->Ratings[i],pamiec->NRatings[i]);
	}
	
	//zakupy
	for (int i=1; i <= N_CUSTOMERS;i++)
	{
       	 	if(fork()==0)
        	{
            		klient1();
			exit(1);
            }
        }
	//czekanie na koniec zaupow
	for (int i=1; i <= N_CUSTOMERS;i++)
	{
		//waitpid(Child_PID[i], NULL, 0);
		wait(NULL);
	}
	
	//wyswietlanie
	printf("\nSrednie oceny kazdego produktu i liczba ich zakupow:\n");
	printf("Towar\tSrednia Ocena\tLiczba kupionych\n");
	for(int i=0; i<N_GOODS; i++)
	{
		printf("%d\t%f\t%d\n", (i+1), pamiec->Ratings[i], pamiec->NRatings[i]);
		kupionych = kupionych+pamiec->NRatings[i];
	}
	printf("\nKupionych: %d\nPowinno byc: %d\n", kupionych, (N_CUSTOMERS*N_ROUNDS));
	return 0;
}

int klient1(){	
	int GOOD, test=0;
	double Rating;
	
	//kupowanie i ocenianie
	for(int i=0; i<N_ROUNDS; i++)
	{
		srand(time(NULL)+i);
		GOOD = rand() % N_GOODS;
		//printf("%d\n", GOOD);
		pamiec->NRatings[GOOD]++;
		//printf("%d\n",pamiec->NRatings[GOOD]);
		srand(time(NULL)+i);
		Rating = rand() % (RATING + 1);
		//printf("%f\n",Rating);
		pamiec->Ratings[GOOD] = ((pamiec->Ratings[GOOD] * (pamiec->NRatings[GOOD] - 1)) + Rating) / pamiec->NRatings[GOOD];
	}
	
	return 0;
}
