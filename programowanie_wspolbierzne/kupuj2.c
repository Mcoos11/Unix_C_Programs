#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
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
	//deklaracja zmiennych w pamieci wspoldzielonej
	int id, test = 0;
	struct wsp_str* Pamiec_WSP = 0;
	
	id = shmget(0x193, MEM_SIZ, 0666 | IPC_CREAT);
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
	
	klient1();
	
	//wyswietlanie
	printf("Srednie oceny kazdego produktu i liczba ich zakupow - klient1:\n");
	printf("Towar\tSrednia Ocena\tLiczba kupionych\n");
	for(int i=0; i<N_GOODS; i++)
		printf("%d\t%f\t%d\n", (i+1), pamiec->Ratings[i], pamiec->NRatings[i]);
	
	//sprawdzanie
	for(int i=0; i<N_GOODS; i++)
	{
		test = test + pamiec->NRatings[i];
	}
	if(test == N_ROUNDS)
	{
		printf("\nLiczba zakupionych towarow: %d\n", test);
		test = 0;
	}
	else
	{
		printf("NIE Dziala. Liczba zkupionych towarow sie nie zgadza: %d\nPowino byc: %d\n", test, N_ROUNDS);
		test = 0;
	}
	
	return 0;
}

int klient1(){	
	int GOOD;
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
		pamiec->Ratings[GOOD] = ((pamiec->Ratings[GOOD] * (pamiec->NRatings[GOOD]-1)) + Rating) / pamiec->NRatings[GOOD];
	}
	


	return 0;
}
